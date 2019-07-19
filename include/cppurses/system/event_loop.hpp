#ifndef CPPURSES_SYSTEM_EVENT_LOOP_HPP
#define CPPURSES_SYSTEM_EVENT_LOOP_HPP
#include <atomic>
#include <future>
#include <thread>

#include <signals/signal.hpp>
#include <signals/slot.hpp>

#include <cppurses/painter/detail/screen.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>

namespace cppurses {

/// Waits on loop_function() and then notifies Event_engine if event is posted.
/** Specialized by providing a loop_function to be run at each iteration. */
class Event_loop {
   public:
    Event_loop() { this->connect_to_system_exit(); }

    /// Make sure the loop has exited and returned from async functions.
    virtual ~Event_loop()
    {
        this->exit(0);
        this->wait();
    }

    /// Start the event loop.
    auto run() -> int;

    /// Start the event loop in a separate thread.
    auto run_async() -> void;

    /// Call on the loop to exit at the next exit point.
    /** The return code value is used when returning from run() or wait(). This
     *  function is thread safe. */
    virtual auto exit(int return_code) -> void
    {
        return_code_ = return_code;
        exit_        = true;
    }

    /// Block until the async event loop returns.
    /** Event_loop::exit(int) must be called to return from wait().
     *  @return the return code passed to the call to exit(). */
    auto wait() -> int;

   protected:
    /// Override this in derived classes to define Event_loop behavior.
    /** This function will be called on once every loop iteration. If it posts
     *  any events via System::post_event(), then it should return true,
     *  indicating that the event queue should be processed. */
    virtual auto loop_function() -> bool = 0;

    bool is_main_thread_{false};

   private:
    /// Connect to the System::exit_signal so loop can exit with System.
    auto connect_to_system_exit() -> void;

    std::future<int> fut_;
    int return_code_{0};
    std::atomic<bool> exit_{false};
    bool running_{false};

    sig::Signal<void()> lifetime;

    friend class System;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENT_LOOP_HPP
