#ifndef CPPURSES_SYSTEM_SYSTEM_HPP
#define CPPURSES_SYSTEM_SYSTEM_HPP
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

#include <signals/signal.hpp>
#include <signals/slot.hpp>

#include <cppurses/system/detail/is_sendable.hpp>
#include <cppurses/system/detail/user_input_event_loop.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/terminal/terminal.hpp>

namespace cppurses {
class Animation_engine;
class Widget;

/// Organizes the highest level of the TUI framework.
/** Constructing an instance of this class initializes the display system.
 *  Manages the head Widget and the main User_input_event_loop. */
class System {
   public:
    System()              = default;
    System(const System&) = delete;
    System& operator=(const System&) = delete;
    System(System&&)                 = default;
    System& operator=(System&&) = default;
    ~System();

    /// Set a new head Widget for the entire system.
    /** Will disable the previous head widget and enable \p new_head, if not
     *  nullptr. Will also send Resize_event and Paint_events to \p new_head. */
    static void set_head(Widget* new_head);

    /// Return a pointer to the head Widget.
    /** This Widget is the ancestor of every other widget that will be displayed
     *  on the screen. */
    static Widget* head() { return head_; }

    /// Set the Widget to receive focus on run().
    /** Needed because focus has to be set after a widget is enabled. */
    static auto set_initial_focus(Widget* target) -> void
    {
        initial_focus_ = target;
    }

    /// Create a Widget_t object, set it as head widget and call System::run().
    /** \p args... are passed on to the Widget_t constructor. Blocks until
     *  System::exit() is called, returns the exit code. Will throw a
     *  std::runtime_error if screen cannot be initialized. */
    template <typename Widget_t, typename... Args>
    int run(Args&&... args)
    {
        Widget_t head_widget(std::forward<Args>(args)...);
        System::set_head(&head_widget);
        return this->run();
    }

    /// Set \p head as head widget and call System::run().
    /** Will throw a std::runtime_error if screen cannot be initialized. */
    int run(Widget& head);

    /// Launch the main Event_loop and start processing Events.
    /** Blocks until System::exit() is called, returns the exit code. Will throw
     *  a std::runtime_error if screen cannot be initialized. */
    int run();

    /// Immediately send the event filters and then to the intended receiver.
    static auto send_event(const Event& event) -> bool
    {
        if (!detail::is_sendable(event))
            return false;
        bool handled = event.send_to_all_filters();
        if (!handled)
            handled = event.send();
        return handled;
    }

    /// Append the event to the Event_queue for the thread it was called on.
    /** The Event_queue is processed once per iteration of the Event_loop. When
     *  the Event is pulled from the Event_queue, it is processed by
     *  System::send_event() */
    static void post_event(std::unique_ptr<Event> event);

    /// Append a newly created Event of type T onto the Event_queue.
    /** \p args... are passed onto the constructor of T. Has same behavior as
     *  the non-templated function of the same name once the object has been
     *  constructed. */
    template <typename T, typename... Args>
    static void post_event(Args&&... args)
    {
        auto event = std::make_unique<T>(std::forward<Args>(args)...);
        System::post_event(std::move(event));
    }

    /// Send an exit signal to each of the currently running Event_loops.
    /** Also call shutdown() on the Animation_engine and set
     *  System::exit_requested_ to true. Though it sends the exit signal to each
     *  of the Event_loops, they are not guaranteed to be stopped by the time
     *  this function returns. */
    static void exit(int exit_code = 0);

    /// Return a reference to the Animation_engine in System.
    /** This manages animation on each of the Widgets that enables it. */
    static Animation_engine& animation_engine() { return animation_engine_; }

    /// Return whether System has gotten an exit request, set by System::exit()
    static bool exit_requested() { return exit_requested_; }

    /// Emitted when System::exit is called. Should call Event_loop::exit.
    /** Passes along the exit_code System::exit() was called with. */
    static sig::Signal<void(int)> exit_signal;

    // Slots
    static sig::Slot<void()> quit;

   private:
    static Widget* head_;
    static Widget* initial_focus_;
    static bool exit_requested_;
    static detail::User_input_event_loop user_input_loop_;
    static Animation_engine animation_engine_;

   public:
    /// Provides access to and modification of global terminal properties.
    static Terminal terminal;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_SYSTEM_HPP
