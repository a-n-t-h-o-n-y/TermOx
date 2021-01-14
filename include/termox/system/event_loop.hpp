#ifndef TERMOX_SYSTEM_EVENT_LOOP_HPP
#define TERMOX_SYSTEM_EVENT_LOOP_HPP
#include <atomic>
#include <future>
#include <stdexcept>
#include <thread>
#include <utility>

#include <signals_light/signal.hpp>

#include <termox/painter/detail/screen.hpp>
#include <termox/painter/detail/staged_changes.hpp>

namespace ox {

/// Waits on loop_function() and then notifies Event_engine if event is posted.
/** Specialized by providing a loop_function to be run at each iteration. The
 *  owner of the event loop is responsible for making sure its async thread is
 *  shutdown: exit() then wait(). */
class Event_loop {
   public:
    Event_loop() { this->connect_to_system_exit(); }

    /// Make sure the loop has exited and returned from async functions.
    virtual ~Event_loop() = default;

    /// Start the event loop.
    auto run() -> int;

    /// Start the event loop in a separate thread.
    void run_async()
    {
        fut_ = std::async(std::launch::async, [this] { return this->run(); });
    }

    /// Call on the loop to exit at the next exit point.
    /** The return code value is used when returning from run() or wait(). This
     *  will wait for the calling loop_function to return before exiting.
     *  Implement a timeout loop_function() if you need to exit quickly. Not
     *  valid to call this method if run() is not currently executing. Only
     *  valid to call once per call to run(). */
    void exit(int return_code)
    {
        return_code_ = return_code;
        exit_        = true;
    }

    /// Block until the async event loop returns.
    /** Event_loop::exit(int) must be called to return from wait().
     *  @return the return code passed to the call to exit(). */
    auto wait() -> int { return fut_.valid() ? fut_.get() : -1; }

   protected:
    /// Override this in derived classes to define Event_loop behavior.
    /** This function will be called on once every loop iteration. */
    virtual void loop_function() = 0;

    /// Return true if the exit flag has been set.
    auto exit_flag() const -> bool { return exit_; }

   private:
    /// Connect to the System::exit_signal so loop can exit with System.
    auto connect_to_system_exit() -> void;

   private:
    std::future<int> fut_;
    sl::Lifetime lifetime_;
    int return_code_        = 0;
    bool running_           = false;
    std::atomic<bool> exit_ = false;

   protected:
    bool is_main_thread_ = false;

    friend class System;
};

}  // namespace ox
#endif  // TERMOX_SYSTEM_EVENT_LOOP_HPP
