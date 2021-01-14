#include <termox/system/detail/timer_event_loop.hpp>

#include <termox/system/event.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/widget.hpp>

namespace ox::detail {

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
    // The first call to this returns immediately.
    Interval_event_loop::loop_function();
    {
        auto const guard = Guard_t{mtx_registered_widgets_};
        for (Widget* widg : registered_widgets_)
            System::post_event(Timer_event{*widg});
    }
}

}  // namespace ox::detail
