#pragma once

#include <chrono>
#include <map>

#include <zzz/timer_thread.hpp>

namespace ox {
class Widget;

/**
 * A handle to a Timer in the Terminal's timer system.
 * @details This is used to create a new TimerThread with a given ID and provides access
 * to start and stop it. The thread will post Timer events to the event queue at the
 * given interval.
 */
class Timer {
   public:
    /**
     * Create a Timer with the given duration.
     *
     * @param w The Widget to send Timer events to, typically `*this` if owned by Widg.
     * @param duration The periodic duration to wait before calling the callback
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
     * @details Does not wait for thread to exit. The destructor of Terminal will wait
     * for it to exit, or if you call start() again.
     */
    void stop();

    [[nodiscard]] auto id() const -> int { return id_; }

    [[nodiscard]] auto duration() const -> std::chrono::milliseconds
    {
        return duration_;
    }

    [[nodiscard]] auto is_running() const -> bool { return is_running_; }

   private:
    inline static int next_id_ = 0;

    int id_;
    std::chrono::milliseconds duration_;
    zzz::TimerThread timer_thread_;
    bool is_running_ = false;
};

}  // namespace ox