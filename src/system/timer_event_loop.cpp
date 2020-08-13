#include <cppurses/system/detail/timer_event_loop.hpp>

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
    w.destroyed.connect([this, &w] { this->unregister_widget(w); });
}

void Timer_event_loop::loop_function()
{
    {
        auto const guard = Guard_t{mtx_registered_widgets_};
        for (Widget* widg : registered_widgets_)
            System::post_event<Timer_event>(*widg);
    }
    Interval_event_loop::loop_function();
}

}  // namespace cppurses::detail
