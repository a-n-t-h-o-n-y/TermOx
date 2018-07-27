#ifndef CPPURSES_SYSTEM_ANIMATION_ENGINE_HPP
#define CPPURSES_SYSTEM_ANIMATION_ENGINE_HPP
#include <chrono>
#include <functional>
#include <unordered_map>
#include <vector>

#include <cppurses/system/detail/animation_event_loop.hpp>
#include <cppurses/system/detail/chrono_duration_hash.hpp>

namespace cppurses {
class Widget;

/// Manages all Animation_event_loops, grouping by period.
class Animation_engine {
   public:
    using Period_t = detail::Animation_event_loop::Period_t;

    /// Begins posting Animation_events to the given Widget every period.
    void register_widget(Widget& w, Period_t period);

    /// Begins posting Animation_events to the Widget with a variable period.
    void register_widget(Widget& w,
                         const std::function<Period_t()>& period_func);

    /// Stop posting Animation_events to a given Widget.
    void unregister_widget(Widget& w);

    /// Sends stop signals to all event loops and waits for them to exit.
    void shutdown();

    /// Starts sending Animation_events to all registered widgets.
    ///
    /// Only needed if shutdown() has been called.
    void startup();

   private:
    // void clean_up();

    std::unordered_map<Period_t, detail::Animation_event_loop> const_loops_;
    std::vector<detail::Animation_event_loop> variable_loops_;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_ANIMATION_ENGINE_HPP
