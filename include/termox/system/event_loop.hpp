#ifndef TERMOX_SYSTEM_EVENT_LOOP_HPP
#define TERMOX_SYSTEM_EVENT_LOOP_HPP
#include <atomic>
#include <cassert>
#include <future>
#include <stdexcept>
#include <thread>
#include <utility>

#include <signals_light/signal.hpp>

namespace ox {

/// Calls on loop_function(), and then processes the Event_queue.
/** Specialized by providing a loop_function to be run at each iteration. The
 *  owner of the event loop is responsible for making sure its async thread is
 *  shutdown: exit() then wait(). */
class Event_loop {
   public:
    Event_loop() { this->connect_to_system_exit(); }

   public:
    /// Start the event loop, calling \p loop_function on each iteration.
    /** loop_function should have signature: void(). It should probably post an
     *  Event. */
    template <typename F>
    auto run(F loop_function) -> int
    {
        if (running_)
            return -1;
        running_ = true;
        while (!exit_) {
            this->send_all_events_and_flush();
            loop_function();
        }
        running_ = false;
        exit_    = false;
        return return_code_;
    }

    /// Start the event loop in a separate thread.
    /** loop_function should have signature: void(). It should probably post an
     *  Event. */
    template <typename F>
    void run_async(F&& loop_function)
    {
        if (fut_.valid())
            return;
        fut_ = std::async(std::launch::async, [this, loop_function] {
            return this->run(std::move(loop_function));
        });
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

    /// Return true if the event loop is currently running.
    [[nodiscard]] auto is_running() const -> bool { return running_; }

    /// Return true if the exit flag has been set.
    [[nodiscard]] auto exit_flag() const -> bool { return exit_; }

   private:
    /// Connect to the System::exit_signal so loop can exit with System.
    auto connect_to_system_exit() -> void;

    /// Send all events on the event queue and flush the changes to the screen.
    void send_all_events_and_flush();

   private:
    std::future<int> fut_;
    sl::Lifetime lifetime_;
    int return_code_        = 0;
    bool running_           = false;
    std::atomic<bool> exit_ = false;

   protected:
    friend class System;
};

}  // namespace ox
#endif  // TERMOX_SYSTEM_EVENT_LOOP_HPP
