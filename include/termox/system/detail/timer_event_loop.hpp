#ifndef TERMOX_SYSTEM_DETAIL_TIMER_EVENT_LOOP_HPP
#define TERMOX_SYSTEM_DETAIL_TIMER_EVENT_LOOP_HPP
#include <chrono>
#include <functional>
#include <mutex>
#include <set>

#include <termox/system/detail/interval_event_loop.hpp>

namespace ox {
class Widget;
}  // namespace ox

namespace ox::detail {

/// Event loop for sending Timer_events at a given interval.
class Timer_event_loop : public Interval_event_loop {
   private:
    using Mutex_t = std::mutex;
    using Guard_t = std::scoped_lock<Mutex_t>;

   public:
    ~Timer_event_loop()
    {
        // Have to end the thread here, since it uses the std::set<...>
        this->exit(0);
        this->wait();
    }

   public:
    using Interval_event_loop::Interval_event_loop;

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

    /// Remove all registered Widgets from this Event Loop.
    void clear()
    {
        auto const guard = Guard_t{mtx_registered_widgets_};
        registered_widgets_.clear();
    }

    /// Return true if no Widgets are registered with this event loop.
    auto is_empty() const -> bool
    {
        auto const guard = Guard_t{mtx_registered_widgets_};
        return registered_widgets_.empty();
    }

   protected:
    void loop_function() override;

   private:
    std::set<Widget*> registered_widgets_;
    mutable Mutex_t mtx_registered_widgets_;
};

}  // namespace ox::detail
#endif  // TERMOX_SYSTEM_DETAIL_TIMER_EVENT_LOOP_HPP
