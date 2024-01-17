#include <termox/system/animation_engine.hpp>

#include <algorithm>
#include <chrono>
#include <iterator>
#include <utility>
#include <vector>

#include <termox/common/fps.hpp>
#include <termox/system/event.hpp>
#include <termox/system/event_loop.hpp>
#include <termox/system/event_queue.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/widget.hpp>

namespace {

auto timer_events = std::vector<ox::Timer_event>{};

}

namespace ox {

void Animation_engine::register_widget(Widget& w, Duration_t interval)
{
    auto const lock = this->Lockable::lock();
    subjects_.insert(std::pair{&w, Registered_data{interval, Clock_t::now()}});
}

void Animation_engine::register_widget(Widget& w, FPS fps)
{
    this->register_widget(w, fps_to_period<Duration_t>(fps));
}

void Animation_engine::unregister_widget(Widget& w)
{
    auto const lock = this->Lockable::lock();
    subjects_.erase(&w);
}

auto Animation_engine::is_empty() const -> bool { return subjects_.empty(); }

void Animation_engine::start()
{
    loop_.run_async([this](Event_queue& q) { this->loop_function(q); });
}

void Animation_engine::stop()
{
    loop_.exit(0);
    loop_.wait();
}

auto Animation_engine::is_running() const -> bool { return loop_.is_running(); }

void Animation_engine::loop_function(Event_queue& queue)
{
    // The first call to wait() returns immediately.
    timer_.wait();
    timer_.begin();
    for (Timer_event& e : get_timer_events())  // This resets the Timer interval
        queue.append(std::move(e));
}

auto Animation_engine::get_timer_events() -> std::vector<Timer_event>&
{
    timer_events.clear();
    if (subjects_.empty()) {
        timer_.set_interval(default_interval);
        return timer_events;
    }
    auto const lock    = this->Lockable::lock();
    auto next_interval = [&, this] {
        return std::min_element(std::cbegin(subjects_), std::cend(subjects_),
                                [](auto const& a, auto const& b) {
                                    return a.second.interval <
                                           b.second.interval;
                                })
            ->second.interval;
    }();

    auto const now = Clock_t::now();
    for (auto& [widget, data] : subjects_) {
        auto const time_left = std::chrono::duration_cast<Duration_t>(
            data.interval - (now - data.last_event_time));
        if (time_left <= Duration_t{0}) {
            data.last_event_time = now;
            timer_events.push_back(Timer_event{*widget});
        }
        else {
            next_interval = std::min(next_interval, time_left);
        }
    }
    timer_.set_interval(next_interval);
    return timer_events;
}

}  // namespace ox
