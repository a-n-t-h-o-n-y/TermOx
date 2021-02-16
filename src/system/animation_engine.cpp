#include <chrono>
#include <termox/system/animation_engine.hpp>

#include <cassert>
#include <thread>
#include <utility>

#include <termox/system/event.hpp>
#include <termox/system/system.hpp>

namespace ox {

void Animation_engine::register_widget(Widget& w, Interval_t interval)
{
    auto const lock = this->Lockable::lock();
    this->maybe_update_interval(interval);
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
    this->reset_interval();
}

void Animation_engine::loop_function()
{
    Interval_event_loop::loop_function();
    this->post_timer_events();
}

void Animation_engine::post_timer_events()
{
    auto const lock = this->Lockable::lock();
    auto const now  = Clock_t::now();
    for (auto& [widget, data] : subjects_) {
        if (now - data.last_event_time >= data.interval) {
            data.last_event_time = now;
            System::post_event(Timer_event{*widget});
        }
    }
}

}  // namespace ox
