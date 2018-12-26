#include <cppurses/system/detail/timer_event_loop.hpp>

#include <chrono>
#include <iterator>
#include <set>
#include <thread>
#include <utility>

#include <signals/signals.hpp>

#include <cppurses/system/event_loop.hpp>
#include <cppurses/system/events/timer_event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
namespace detail {

void Timer_event_loop::register_widget(Widget& w) {
    registered_widgets_.emplace(&w);
    w.destroyed.connect([this](Widget& d) { registered_widgets_.erase(&d); });
}

void Timer_event_loop::loop_function() {
    for (Widget* widg : registered_widgets_) {
        System::post_event<Timer_event>(*widg);
    }
    auto now = std::chrono::high_resolution_clock::now();
    auto time_passed = now - last_time_;
    auto time_to_sleep = period_func_() - time_passed;
    if (time_to_sleep > Period_t::zero()) {
        std::this_thread::sleep_for(time_to_sleep);
    }
    last_time_ = std::chrono::high_resolution_clock::now();
}

}  // namespace detail
}  // namespace cppurses
