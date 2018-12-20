#ifndef CPPURSES_SYSTEM_SYSTEM_HPP
#define CPPURSES_SYSTEM_SYSTEM_HPP
#include <cstddef>
#include <memory>
#include <mutex>
#include <utility>

#include <signals/slot.hpp>

#include <cppurses/system/detail/user_input_event_loop.hpp>
#include <cppurses/system/terminal_properties.hpp>

namespace cppurses {
class Widget;
class Event;
class Animation_engine;
class Event_loop;

/// Organizes the highest level of the TUI framework.
/** Constructing an instance of this class initializes the display system.
 *  Manages the head Widget and the main User_input_event_loop. */
class System {
   public:
    System();
    System(const System&) = delete;
    System& operator=(const System&) = delete;
    System(System&&) = default;
    System& operator=(System&&) = default;
    ~System();

    /// Launches the main Event_loop and starts processing Events.
    int run();

    /// Immediately sends the Event filters and then to the intended receiver.
    static bool send_event(const Event& event);

    /// Appends the event to the Event_queue for the thread it was called on.
    /** The Event_queue is processed once per iteration of the Event_loop. When
     *  the Event is pulled from the Event_queue, it is processed by
     *  System::send_event() */
    static void post_event(std::unique_ptr<Event> event);

    /// Appends a newly created Event of type T onto the Event_queue.
    /** \p args... are passed onto the constructor of T. Has same behavior as
     *  the non-templated function of the same name once the object has been
     *  constructed. */
    template <typename T, typename... Args>
    static void post_event(Args&&... args) {
        auto event = std::make_unique<T>(std::forward<Args>(args)...);
        System::post_event(std::move(event));
    }

    /// Returns the Event_loop associated with the calling thread.
    /** Each currently running Event_loop has to be run on its own thread, this
     *  function will find and return the Event_loop that is currently running
     *  on the calling thread. Used by Painter to get the staged_changes owned
     *  by Event_loop. */
    static Event_loop& find_event_loop();

    /// Adds an Event_loop* to a list of currently running Event_loops.
    /** Used by Event_loop::run() to automatically register itself to the list
     *  of running Event_loops when the loop begins. */
    static void register_event_loop(Event_loop* loop);

    /// Removes the given Event_loop* from list of running Event_loops.
    /** Used by Event_loop::run() to automatically deregister itself from the
     *  list of running Event_loops when the loops exits. No-op if Event_loop*
     * is not registered. */
    static void deregister_event_loop(Event_loop* loop);

    /// Sends an exit signal to each of the currently running Event_loops.
    /** Also calls shutdown() on the Animation_engine and sets
     *  System::exit_requested_ to true. Though it sends the exit signal to each
     *  of the Event_loops, they are not guaranteed to be stopped by the time
     *  this function returns. */
    static void exit(int return_code = 0);

    /// Returns a pointer to the head Widget.
    /** This is the Widget that has no parent and is the ancestor of every
     *  Widget on the tree of Widgets that are part of System. */
    static Widget* head() { return head_; }

    /// Sets the head Widget for the System.
    /** Calling this function with a new Widget as the head will change the
     *  working set of Widgets that System uses to run the TUI. */
    static void set_head(Widget* head_widget);

    /// Returns a reference to the Animation_engine in System.
    /** This manages animation on each of the Widgets that enables it. */
    static Animation_engine& animation_engine() { return animation_engine_; }

    /// Returns whether System has gotten an exit request, set by System::exit()
    static bool exit_requested() { return exit_requested_; }

    // Slots
    static sig::Slot<void()> quit;

   private:
    static std::vector<Event_loop*> running_event_loops_;
    static std::mutex running_loops_mtx_;

    static Widget* head_;
    static bool exit_requested_;
    static detail::User_input_event_loop main_loop_;
    static Animation_engine animation_engine_;

   public:
    /// Provides access to and modification of global terminal properties.
    /** Unsafe to use if System object has not been initialized */
    static Terminal_properties terminal;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_SYSTEM_HPP
