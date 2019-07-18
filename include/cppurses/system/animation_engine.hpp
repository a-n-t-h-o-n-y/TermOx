#ifndef CPPURSES_SYSTEM_ANIMATION_ENGINE_HPP
#define CPPURSES_SYSTEM_ANIMATION_ENGINE_HPP
#include <functional>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include <cppurses/system/detail/chrono_duration_hash.hpp>
#include <cppurses/system/detail/timer_event_loop.hpp>

namespace cppurses {
class Widget;
class Event_loop;

/// Manages all Timer_event_loops, grouping by period.
class Animation_engine {
   public:
    using Period_t = detail::Timer_event_loop::Period_t;

    /// Begins posting Timer_events to the given Widget every period.
    void register_widget(Widget& w, Period_t period);

    /// Begins posting Timer_events to the Widget with a variable period.
    void register_widget(Widget& w,
                         const std::function<Period_t()>& period_func);

    /// Stop posting Timer_events to a given Widget.
    void unregister_widget(Widget& w);

    /// Send stop signals to all event loops and wait for them to exit.
    void shutdown();

    /// Start sending Timer_events to all registered widgets.
    /** Only needed if shutdown() has been called. */
    void startup();

   private:
    using Loop_t = std::unique_ptr<detail::Timer_event_loop>;
    std::unordered_map<Period_t, Loop_t> const_loops_;
    std::vector<Loop_t> variable_loops_;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_ANIMATION_ENGINE_HPP
