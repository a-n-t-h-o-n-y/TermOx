#ifndef CPPURSES_SYSTEM_DETAIL_TIMER_EVENT_LOOP_HPP
#define CPPURSES_SYSTEM_DETAIL_TIMER_EVENT_LOOP_HPP
#include <chrono>
#include <functional>
#include <mutex>
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
   private:
    using Mutex_t = std::mutex;
    using Guard_t = std::lock_guard<Mutex_t>;
    using Clock_t = std::chrono::high_resolution_clock;

   public:
    using Period_t = std::chrono::milliseconds;

   public:
    /// Create a Timer Loop with a constant period.
    explicit Timer_event_loop(Period_t period)
        : get_period_{[period] { return period; }}
    {}

    /// Create a Timer_loop with a variable period.
    explicit Timer_event_loop(std::function<Period_t()> period_func)
        : get_period_{period_func}
    {}

    /// Register a widget to have a Timer_event posted to it every period.
    /** No-op if widget is already registered. */
    void register_widget(Widget& w);

    /// Stop a widget from receiving Timer_events for this loop.
    /** Returns true if \p w was found and unregistered. */
    auto unregister_widget(Widget& w) -> bool
    {
        auto const guard = Guard_t{mtx_registered_widgets_};
        return registered_widgets_.erase(&w) == 1;
    }

    /// Set a new constant period for the Timer loop.
    void set_period(Period_t period)
    {
        auto const guard = Guard_t{mtx_get_period_};
        get_period_      = [period] { return period; };
    }

    /// Set a new variable period for the Timer loop.
    void set_period(std::function<Period_t()> const& period_function)
    {
        auto const guard = Guard_t{mtx_get_period_};
        get_period_      = period_function;
    }

    /// Return true if no Widgets are registered with this event loop.
    auto empty() const -> bool
    {
        auto const guard = Guard_t{mtx_registered_widgets_};
        return registered_widgets_.empty();
    }

   protected:
    auto loop_function() -> bool override;

   private:
    std::set<Widget*> registered_widgets_;
    mutable Mutex_t mtx_registered_widgets_;

    std::function<Period_t()> get_period_;
    mutable Mutex_t mtx_get_period_;

    std::chrono::time_point<Clock_t> last_time_;

   private:
    void wait_on_timer();
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_DETAIL_TIMER_EVENT_LOOP_HPP
