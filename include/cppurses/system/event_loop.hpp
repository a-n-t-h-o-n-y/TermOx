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

/// Waits on loop_function() and then processes the event queue.
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
    int run();

    /// Start the event loop in a separate thread.
    void run_async();

    /// Call on the loop to exit at the next exit point.
    /** The return code value is used when returning from run() or wait(). This
     *  function is thread safe. */
    virtual void exit(int return_code)
    {
        return_code_ = return_code;
        exit_        = true;
    }

    /// Block until the async event loop returns.
    /** Event_loop::exit(int) must be called to return from wait().
     *  @return the return code passed to the call to exit(). */
    int wait();

   protected:
    /// Override this in derived classes to define Event_loop behavior.
    /** This function will be called on once every loop iteration. It is
     *  expected that is will post an event to the Event_queue. After this
     *  function is called, the Event_queue is invoked, and then staged changes
     *  for this Event_loop are flushed to the screen, and the loop begins
     *  again. */
    virtual void loop_function() = 0;

   private:
    void process_events();

    std::future<int> fut_;
    int return_code_{0};
    std::atomic<bool> exit_{false};
    bool running_{false};

    sig::Signal<void()> lifetime;

    friend class System;

    /// Connect to the System::exit_signal so loop can exit with System.
    auto connect_to_system_exit() -> void;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENT_LOOP_HPP
