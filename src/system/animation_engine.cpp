#include <termox/system/animation_engine.hpp>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <thread>
#include <utility>
#include <vector>

#include <termox/system/event.hpp>
#include <termox/system/system.hpp>

namespace {

auto timer_events = std::vector<ox::Timer_event>{};

}

namespace ox {

void Animation_engine::register_widget(Widget& w, Interval_t interval)
{
    auto const lock = this->Lockable::lock();
    subjects_.insert(std::pair{&w, Registered_data{interval, Clock_t::now()}});
}

void Animation_engine::register_widget(Widget& w, FPS fps)
{
    this->register_widget(w, Interval_event_loop::fps_to_period(fps));
}

void Animation_engine::unregister_widget(Widget& w)
{
    auto const lock = this->Lockable::lock();
    subjects_.erase(&w);
}

void Animation_engine::loop_function()
{
    // The first call to this returns immediately.
    Interval_event_loop::loop_function();
    this->post_timer_events();
}

void Animation_engine::post_timer_events()
{
    if (subjects_.empty()) {
        this->set_interval(default_interval);
        return;
    }

    timer_events.clear();
    {
        auto const lock    = this->Lockable::lock();
        auto next_interval = [&, this] {
            return std::min_element(
                       std::cbegin(subjects_), std::cend(subjects_),
                       [](auto const& a, auto const& b) {
                           return a.second.interval < b.second.interval;
                       })
                ->second.interval;
        }();

        auto const now = Clock_t::now();
        for (auto& [widget, data] : subjects_) {
            auto const time_left = std::chrono::duration_cast<Interval_t>(
                data.interval - (now - data.last_event_time));
            if (time_left <= Interval_t{0}) {
                data.last_event_time = now;
                timer_events.push_back(Timer_event{*widget});
            }
            else {
                next_interval = std::min(next_interval, time_left);
            }
        }
        this->Interval_event_loop::set_interval(next_interval);
    }
    // Send events without a lock on the mutex.
    for (auto const& e : timer_events)
        System::post_event(e);
}

}  // namespace ox
