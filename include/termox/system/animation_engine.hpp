#ifndef TERMOX_SYSTEM_ANIMATION_ENGINE_HPP
#define TERMOX_SYSTEM_ANIMATION_ENGINE_HPP
#include <memory>
#include <vector>

#include <termox/system/detail/timer_event_loop.hpp>

namespace ox {
class Widget;

/// Manages all Timer_event_loops, grouping by period.
class Animation_engine {
   public:
    using Period_t = detail::Timer_event_loop::Period_t;

   public:
    ~Animation_engine()
    {
        for (auto& loop : loops_) {
            loop->exit(0);
            loop->wait();
        }
    }

   public:
    /// Begins posting Timer_events to the given Widget every period.
    void register_widget(Widget& w, Period_t interval);

    /// Begins posting Timer_events to the given Widget at \p fps.
    void register_widget(Widget& w, FPS fps);

    /// Stop posting Timer_events to a given Widget.
    void unregister_widget(Widget& w);

    // Start sending Timer_events to all registered widgets.
    /** Only needed if shutdown() has been called. */
    void startup();

    /// Send stop signals to all event loops and wait for them to exit.
    void shutdown();

   private:
    // Using a std::unique_ptr because Event_loops can't be copied.
    using Loop_t = std::unique_ptr<detail::Timer_event_loop>;
    std::vector<Loop_t> loops_;

   private:
    /// Find and return iterator pointing to Event Loop with \p interval.
    /** Returns std::end(loops_) if no loop found with \p interval. */
    auto get_loop_iter_with(Period_t interval) const
    {
        return std::find_if(std::begin(loops_), std::end(loops_),
                            [interval](auto const& loop) {
                                return loop->get_interval() == interval;
                            });
    }

    /// Return true if there is already an Event_loop for \p interval.
    auto has_loop_with(Period_t interval) const -> bool
    {
        return this->get_loop_iter_with(interval) != std::end(loops_);
    }

    /// Assumes that the Event Loop does exist, otherwise undefined behavior.
    auto get_loop_with(Period_t interval) -> detail::Timer_event_loop&
    {
        return **this->get_loop_iter_with(interval);
    }
};

}  // namespace ox
#endif  // TERMOX_SYSTEM_ANIMATION_ENGINE_HPP
