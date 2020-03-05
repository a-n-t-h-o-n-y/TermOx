#include <cppurses/system/detail/timer_event_loop.hpp>

#include <algorithm>
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

namespace cppurses::detail {

void Timer_event_loop::register_widget(Widget& w)
{
    {
        auto const guard = Guard_t{mtx_registered_widgets_};
        registered_widgets_.emplace(&w);
    }
    w.destroyed.connect([this](Widget& w) { this->unregister_widget(w); });
}

auto Timer_event_loop::loop_function() -> bool
{
    {
        auto const guard = Guard_t{mtx_registered_widgets_};
        for (Widget* widg : registered_widgets_) {
            System::post_event<Timer_event>(*widg);
        }
    }
    this->wait_on_timer();
    return not this->empty();
}

void Timer_event_loop::wait_on_timer()
{
    auto const time_to_sleep = [this] {
        auto const now         = Clock_t::now();
        auto const time_passed = now - last_time_;
        auto const guard       = Guard_t{mtx_get_period_};
        return std::max(Clock_t::duration::zero(), get_period_() - time_passed);
    }();
    std::this_thread::sleep_for(time_to_sleep);
    last_time_ = Clock_t::now();
}

}  // namespace cppurses::detail
