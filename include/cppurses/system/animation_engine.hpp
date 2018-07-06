#ifndef CPPURSES_SYSTEM_ANIMATION_ENGINE_HPP
#define CPPURSES_SYSTEM_ANIMATION_ENGINE_HPP
#include <functional>
#include <map>
#include <vector>

#include <signals/connection.hpp>

#include <cppurses/system/async_trigger.hpp>

namespace cppurses {
class Widget;

/// Sets up a trigger for any widget to have animation_event called at a
/// given framerate. Keeps track of which Widgets are currently animated and
/// will not double up animations if registered multiple times without
/// deregistering first.
class Animation_engine {
   public:
    /// Begins animation calls to the given widget at the given framerate.
    // Checks first that the widget does not have an active connection.
    void register_widget(Widget* widg, int frames_per_second);

    /// Begins animation calls to the given widget with the function
    /// frames_per_second used to get the fps on each frame.
    // Checks first that the widget does not have an active connection.
    void register_widget(Widget* widg, std::function<int()> frames_per_second);

    /// Stops a widget from being animated
    void deregister_widget(Widget* widg);

    /// sends stop signals to all async triggers and waits for them to shutdown.
    void shutdown();

   private:
    std::map<int, Async_trigger> const_framerate_triggers_;
    std::vector<Async_trigger> variable_framerate_triggers_;
    std::map<Widget*, sig::Connection> connections_;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_ANIMATION_ENGINE_HPP
