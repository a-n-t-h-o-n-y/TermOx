#ifndef TERMOX_COMMON_TIMER_HPP
#define TERMOX_COMMON_TIMER_HPP
#include <chrono>

#include <termox/common/fps.hpp>

namespace ox {

/// Timer class where begin() and wait() are used to block for a given interval.
class Timer {
   public:
    using Clock_t    = std::chrono::steady_clock;
    using Time_point = Clock_t::time_point;
    using Duration_t = std::chrono::milliseconds;

   public:
    /// Construct a Timer with the given interval.
    explicit constexpr Timer(Duration_t interval) : interval_{interval} {}

    /// Construct a Timer with the given FPS interval.
    explicit constexpr Timer(FPS fps)
        : interval_{fps_to_period<Duration_t>(fps)}
    {}

   public:
    /// Start the timer, returns immediately.
    void begin();

    /// Sleep until the time interval is over, from last begin() call.
    /** Returns immediately if interval has already elapsed. Calling this
     *  without calling begin() first will return immediately. */
    void wait();

    /// Set the amount of time to wait for from begin().
    void set_interval(Duration_t interval);

    /// Return the currently set interval.
    [[nodiscard]] auto get_interval() const -> Duration_t;

   private:
    Duration_t interval_;
    Time_point last_time_;

   private:
    /// Return the time to sleep, interval minus time since begin() called.
    /** Returns zero time if that would be negative. */
    [[nodiscard]] auto get_sleep_time() const -> Clock_t::duration;
};

}  // namespace ox
#endif  // TERMOX_COMMON_TIMER_HPP
