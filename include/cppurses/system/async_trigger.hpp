#ifndef CPPURSES_SYSTEM_ASYNC_TRIGGER_HPP
#define CPPURSES_SYSTEM_ASYNC_TRIGGER_HPP
#include <atomic>
#include <functional>
#include <future>
#include <thread>

#include <signals/signals.hpp>

namespace cppurses {

/// Provides a Signal which is called periodically in a separate thread.
/// Requires sig::Signals library and pthread. Period has nanosecond resolution.
/// The period is able to vary on each call determined by a function passed
/// in by the client.
class Async_trigger {
   public:
    /// Resolution of period.
    using Duration_t = std::chrono::nanoseconds;

    Async_trigger() = default;

    /// Initialize with a specific period.
    Async_trigger(Duration_t period);

    /// Initialize with a specific period.
    Async_trigger(const std::function<Duration_t()>& period_function);

    Async_trigger(Async_trigger&& other);
    Async_trigger& operator=(Async_trigger&& other);

    /// Sends a stop signal and waits for the last call of trigger() to return.
    ~Async_trigger();

    /// Starts the periodic triggering in a separate thread. returns
    /// immediately.
    void start();

    /// Sends a stop signal for the async trigger thread. Will stop after
    /// current trigger() call is finished.
    void stop();

    /// Change the period between calls to trigger() to a static Duration_t.
    void set_period(Duration_t period);

    /// Specify a function that determines the duration of the period, called on
    /// each iteration. Allows for variable period.
    void set_period(const std::function<Duration_t()>& period_function);

    /// Returns the period's duration.
    Duration_t get_period() const;

    /// Returns a reference to the function that determines the period.
    std::function<Duration_t()>& get_period_function();

    /// Returns a copy of the function that determines the period.
    std::function<Duration_t()> get_period_function() const;

    /// trigger Signal will be called once every period. This is where Slots
    /// can be attached. Is thread safe.
    sig::Signal<void()> trigger;

    /// waits on the async future to return. stop signal must have been sent.
    void wait();

   private:
    std::function<Duration_t()> func_{[] { return std::chrono::seconds(1); }};
    mutable std::mutex func_mutex_;

    bool looping_{false};

    /// Destructor will wait on this future.
    std::future<void> future_;

    /// loop run asynchronously to call trigger() every period.
    void loop();
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_ASYNC_TRIGGER_HPP
