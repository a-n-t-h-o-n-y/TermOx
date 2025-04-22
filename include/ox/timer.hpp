#pragma once

#include <chrono>
#include <map>

#include <zzz/timer_thread.hpp>

namespace ox {
class Widget;

/**
 * Posts `event::Timer` objects at regular intervals to the EventQueue.
 *
 * @details This uses its own internal thread to enqueue Events. Each Timer is tied to a
 * single Widget, which will recieve the Events. Timer events are handled with the
 * `Widget::timer()` virtual function, typically you would update the Widget state, the
 * `Widget::paint(Canvas)` event handler will be called automatically after the timer
 * event handler.
 *
 * The Widget lifetime is handled by a `LifetimeView` and will remain valid after moving
 * a Widget. Though not recommended for clarity, it is safe to delete a Widget without
 * deleting its associated Timer.
 */
class Timer {
   public:
    /**
     * Create a Timer with the given duration.
     *
     * @param w The Widget to send Timer events to, typically `*this` if owned by Widg.
     * @param duration The interval to wait before posting an Event.
     * @param launch If true, the TimerThread will be started immediately.
     */
    explicit Timer(Widget& w, std::chrono::milliseconds duration, bool launch = false);

    Timer(Timer const&) = delete;
    Timer(Timer&& other);

    auto operator=(Timer const&) -> Timer& = delete;
    auto operator=(Timer&& other) -> Timer&;

    /**
     * Stop the TimerThread if it is running.
     */
    ~Timer();

   public:
    /**
     * Start the timer thread with the given duration.
     * @details This will create and launch a new thread.
     */
    void start();

    /**
     * Request the TimerThread to stop, returns immediately.
     * @details Does not wait for thread to exit.
     */
    void stop();

    [[nodiscard]] auto id() const -> int { return id_; }

    [[nodiscard]] auto duration() const -> std::chrono::milliseconds
    {
        return duration_;
    }

    /**
     * Returns true if `start()` has been called and the Timer is currently running.
     * @details i.e., `stop()` has not been called since the last `start()`.
     */
    [[nodiscard]] auto is_running() const -> bool { return is_running_; }

   private:
    inline static int next_id_ = 0;

    int id_;
    std::chrono::milliseconds duration_;
    zzz::TimerThread timer_thread_;
    bool is_running_ = false;
};

}  // namespace ox