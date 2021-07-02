#ifndef TERMOX_SYSTEM_SYSTEM_HPP
#define TERMOX_SYSTEM_SYSTEM_HPP
#include <atomic>
#include <functional>
#include <utility>

#include <signals_light/signal.hpp>

#include <termox/system/animation_engine.hpp>
#include <termox/system/detail/user_input_event_loop.hpp>
#include <termox/system/event_fwd.hpp>
#include <termox/terminal/key_mode.hpp>
#include <termox/terminal/mouse_mode.hpp>
#include <termox/terminal/signals.hpp>
#include <termox/widget/cursor.hpp>

namespace ox {
class Widget;
class Event_queue;
}  // namespace ox

namespace ox {

/// Organizes the highest level of the TUI framework.
/** Constructing an instance of this class initializes the display system.
 *  Manages the head Widget and the main User_input_event_loop. */
class System {
   public:
    static sl::Slot<void()> quit;

   public:
    /// Initializes the terminal screen into curses mode.
    /** Must be called before any input/output can occur. No-op if initialized.
     *  Mouse_mode  - - Off:   Generates no Mouse Events.
     *                  Basic: Generate Mouse Press and Release Events for all
     *                         buttons and the scroll wheel.
     *                  Drag:  Basic, plus Mouse Move Events while a button is
     *                         pressed.
     *                  Move:  Basic, plus Mouse Move Events are generated with
     *                         or without a button pressed.
     *
     *  Key_mode - Normal: Key_press Events generated and auto-repeated if key
     *                     is held down.
     *             Raw:    Key_press and Key_release Events are generated, the
     *                     shift key is not applied with other keys, each key
     *                     press and release is its own event. Useful for games
     *                     and where you need to keep track of multiple keys
     *                     held down at once. All keys returned in Raw mode are
     *                     lower-case. Has only been tested on a single laptop
     *                     keyboard.
     *
     *  Signals - - - On:  Signals can be generated from ctrl-[key] presses, for
     *                     instance ctrl-c will send SIGINT instead of byte 3.
     *                Off: Signals will not be generated on ctrl-[key] presses,
     *                     sending the byte value of the ctrl character instead.
     */
    System(Mouse_mode mouse_mode = Mouse_mode::Basic,
           Key_mode key_mode     = Key_mode::Normal,
           Signals signals       = Signals::On);

    System(System const&) = delete;
    System& operator=(System const&) = delete;
    System(System&&)                 = default;
    System& operator=(System&&) = default;

    [[noreturn]] ~System();

   public:
    /// Return a pointer to the currently focused Widget.
    [[nodiscard]] static auto focus_widget() -> Widget*;

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
    [[nodiscard]] static auto head() -> Widget*;

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
    auto run(Widget& head) -> int;

    /// Launch the main Event_loop and start processing Events.
    /** Blocks until System::exit() is called, returns the exit code. Will throw
     *  a std::runtime_error if screen cannot be initialized. Enables and sets
     *  focus to the head Widget.*/
    static auto run() -> int;

    /// Immediately send the event filters and then to the intended receiver.
    /** Return true if the event was actually sent. */
    static auto send_event(Event e) -> bool;

    // Minor optimization.
    /** Return true if the event was actually sent. */
    static auto send_event(Paint_event e) -> bool;

    // Minor optimization.
    /** Return true if the event was actually sent. */
    static auto send_event(Delete_event e) -> bool;

    /// Append the event to the Event_queue for the thread it was called on.
    /** The Event_queue is processed once per iteration of the Event_loop. When
     *  the Event is pulled from the Event_queue, it is processed by
     *  System::send_event() */
    static void post_event(Event e);

    /// Sets the exit flag for the user input event loop.
    /** Only call from the main user input event loop, not animation loop. This
     *  is because shutdown will be blocked until more user input is entered.
     *  This calls std::_Exit, does not clean up with destructors.
     *  TODO threading design makes this difficult to do properly. */
    [[noreturn]] static void exit();

    /// Enable animation for the given Widget \p w at \p interval.
    /** Starts the animation_engine if not started yet. */
    static void enable_animation(Widget& w,
                                 Animation_engine::Duration_t interval);

    /// Enable animation for the given Widget \p w at \p fps.
    /** Starts the animation_engine if not started yet. */
    static void enable_animation(Widget& w, FPS fps);

    /// Disable animation for the given Widget \p w.
    /** Does not stop the animation_engine, even if its empty. */
    static void disable_animation(Widget& w);

    /// Set the terminal cursor via \p cursor parameters and \p offset applied.
    static void set_cursor(Cursor cursor, Point offset);

    /// Set the Event_queue that will be used by post_event.
    /** Set by Event_queue::send_all. */
    static void set_current_queue(Event_queue& queue);

   private:
    inline static std::atomic<Widget*> head_ = nullptr;
    static detail::User_input_event_loop user_input_loop_;
    static Animation_engine animation_engine_;
    static std::reference_wrapper<Event_queue> current_queue_;
};

}  // namespace ox
#endif  // TERMOX_SYSTEM_SYSTEM_HPP
