#ifndef TERMOX_SYSTEM_SYSTEM_HPP
#define TERMOX_SYSTEM_SYSTEM_HPP
#include <mutex>
#include <utility>
#include <vector>

#include <signals_light/signal.hpp>

#include <termox/system/detail/user_input_event_loop.hpp>
#include <termox/system/event_fwd.hpp>
#include <termox/terminal/terminal.hpp>

namespace ox {
class Animation_engine;
class Widget;
}  // namespace ox

namespace ox::detail {
class Event_engine;
}  // namespace ox::detail

namespace ox {

/// Organizes the highest level of the TUI framework.
/** Constructing an instance of this class initializes the display system.
 *  Manages the head Widget and the main User_input_event_loop. */
class System {
   public:
    /// Emitted when System::exit is called. Should call Event_loop::exit.
    /** Passes along the exit_code System::exit() was called with. */
    static sl::Signal<void(int)> exit_signal;

    // Slots
    static sl::Slot<void()> quit;

    /// Provides access to and modification of global terminal properties.
    inline static Terminal terminal;

   public:
    System()              = default;
    System(System const&) = delete;
    System& operator=(System const&) = delete;
    System(System&&)                 = default;
    System& operator=(System&&) = default;

    ~System() { System::exit(0); }

    /// Return a pointer to the currently focused Widget.
    static auto focus_widget() -> Widget*;

    /// Give program focus to \p w.
    /** Sends Focus_out_event to Widget in focus, and Focus_in_event to \p w. */
    static void set_focus(Widget& w);

    /// Removes focus from the currently in focus Widget.
    static void clear_focus();

    /// Enable Tab/Back_tab keys to change the focus Widget.
    static void enable_tab_focus();

    /// Disable Tab/Back_tab keys from changing focus Widget.
    static void disable_tab_focus();

    /// Set a new head Widget for the entire system.
    /** Will disable the previous head widget if not nullptr. Only valid to call
     *  before System::run or after System::exit. */
    static void set_head(Widget* new_head);

    /// Return a pointer to the head Widget.
    /** This Widget is the ancestor of every other widget that will be displayed
     *  on the screen. */
    static auto head() -> Widget* { return head_; }

    /// Create a Widget_t object, set it as head widget and call System::run().
    /** \p args... are passed on to the Widget_t constructor. Blocks until
     *  System::exit() is called, returns the exit code. Will throw a
     *  std::runtime_error if screen cannot be initialized. */
    template <typename Widget_t, typename... Args>
    auto run(Args&&... args) -> int
    {
        auto head = Widget_t(std::forward<Args>(args)...);
        System::set_head(&head);
        return this->run();
    }

    /// Set \p head as head widget and call System::run().
    /** Will throw a std::runtime_error if screen cannot be initialized. */
    auto run(Widget& head) -> int
    {
        System::set_head(&head);
        return this->run();
    }

    /// Launch the main Event_loop and start processing Events.
    /** Blocks until System::exit() is called, returns the exit code. Will throw
     *  a std::runtime_error if screen cannot be initialized. Enables and sets
     *  focus to the head Widget.*/
    static auto run() -> int;

    /// Immediately send the event filters and then to the intended receiver.
    static void send_event(Event e);

    // Minor optimization.
    static void send_event(Paint_event e);

    // Minor optimization.
    static void send_event(Delete_event e);

    /// Append the event to the Event_queue for the thread it was called on.
    /** The Event_queue is processed once per iteration of the Event_loop. When
     *  the Event is pulled from the Event_queue, it is processed by
     *  System::send_event() */
    static void post_event(Event e);

    /// Send an exit signal to each of the currently running Event_loops.
    /** Also call shutdown() on the Animation_engine and set
     *  System::exit_requested_ to true. Though it sends the exit signal to each
     *  of the Event_loops, they are not guaranteed to be stopped by the time
     *  this function returns. */
    static void exit(int exit_code = 0);

    /// Returns a reference to the single Event_engine that owns the queue.
    static auto event_engine() -> detail::Event_engine&
    {
        return event_engine_;
    }

    /// Return a reference to the Animation_engine in System.
    /** This manages animation on each of the Widgets that enables it. */
    static auto animation_engine() -> Animation_engine&
    {
        return animation_engine_;
    }

    /// Return whether System has gotten an exit request, set by System::exit()
    static auto exit_requested() -> bool { return exit_requested_; }

   private:
    inline static Widget* head_        = nullptr;
    inline static bool exit_requested_ = false;
    static detail::User_input_event_loop user_input_loop_;
    static detail::Event_engine event_engine_;
    static Animation_engine animation_engine_;
};

}  // namespace ox
#endif  // TERMOX_SYSTEM_SYSTEM_HPP
