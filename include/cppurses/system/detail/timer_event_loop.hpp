#ifndef CPPURSES_SYSTEM_DETAIL_TIMER_EVENT_LOOP_HPP
#define CPPURSES_SYSTEM_DETAIL_TIMER_EVENT_LOOP_HPP
#include <chrono>
#include <functional>
#include <set>

#include <signals/connection.hpp>
#include <signals/signal.hpp>

#include <cppurses/system/event_loop.hpp>

namespace cppurses {
class Widget;
namespace detail {

/// Implements an Event_loop associated with a particular Period_t.
/** Sends Timer_events every period from its own thread. */
class Timer_event_loop : public Event_loop {
   public:
    using Period_t = std::chrono::milliseconds;

    /// Create a Timer Loop with a constant period.
    explicit Timer_event_loop(Period_t period)
        : period_func_{[period] { return period; }}
    {}

    /// Create a Timer_loop with a variable period.
    explicit Timer_event_loop(std::function<Period_t()> period_func)
        : period_func_{period_func}
    {}

    /// Register a widget to have a Timer_event posted to it every period.
    /** No-op if widget is already registered. */
    void register_widget(Widget& w);

    /// Stop a widget from receiving Timer_events for this loop.
    bool unregister_widget(Widget& w)
    {
        return registered_widgets_.erase(&w) == 1;
    }

    /// Set a new constant period for the Timer loop.
    void set_period(Period_t period)
    {
        period_func_ = [period] { return period; };
    }

    /// Set a new variable period for the Timer loop.
    void set_period(const std::function<Period_t()>& period_function)
    {
        period_func_ = period_function;
    }

    /// Return true if no Widgets are registered with this event loop.
    bool empty() const { return registered_widgets_.empty(); }

   protected:
    auto loop_function() -> bool override;

   private:
    std::set<Widget*> registered_widgets_;
    std::function<Period_t()> period_func_;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_time_;
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_TIMER_EVENT_LOOP_HPP
