/** @file
 * Retained timer handles and move-aware widget callbacks.
 */
#pragma once

#include <chrono>
#include <concepts>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <type_traits>

#include <ox/core/events.hpp>

namespace ox {
class Widget;
class EventLoop;
class Application;

/** A movable timer handle. The owning event loop executes its callback. */
class Timer {
   public:
    /** Monotonic clock used by timer deadlines. */
    using Clock = std::chrono::steady_clock;

    /** Create a generic timer; register it with EventLoop::watch() to execute callbacks.
     * The callback runs on the loop thread. launch schedules the first deadline at now + interval.
     * @throws std::invalid_argument The interval is nonpositive or the callback is empty.
     */
    Timer(std::chrono::milliseconds interval,
          std::function<EventResponse()> action,
          bool launch = true);

    /** Create a tracked widget timer, automatically adopted when the widget is attached.
     * The callback receives the widget at its current address. Hidden, disabled,
     * or expired targets are skipped; moving the widget preserves the target.
     * Detaching from an application stops and unregisters its timers.
     * The default launch=false leaves the timer stopped. Keep the timer handle alive.
     * @throws std::invalid_argument The interval is nonpositive or the callback is empty.
     */
    template <typename W, typename Callback>
        requires std::derived_from<W, Widget> &&
                 std::is_invocable_r_v<EventResponse, Callback&, W&>
    Timer(W& widget,
          std::chrono::milliseconds interval,
          Callback action,
          bool launch = false);

    /** Timer handles cannot be copied; one retained handle owns the schedule. */
    Timer(Timer const&) = delete;
    /** Transfer the schedule without changing its event-loop registration. */
    Timer(Timer&&) = default;
    /** Timer handles cannot be copy-assigned. */
    auto operator=(Timer const&) -> Timer& = delete;
    /** Release this schedule and take the source timer's schedule. */
    auto operator=(Timer&&) -> Timer& = default;

    /** Start or restart the deadline at now + duration(). @throws std::logic_error The
     * timer was moved from.
     */
    void start(Clock::time_point now = Clock::now());
    /** Cancel the pending deadline; safe for a moved-from or already stopped timer. */
    void stop();
    /** Return whether a deadline is scheduled, including before event-loop registration. */
    [[nodiscard]] auto is_running() const -> bool;
    /** Return the fixed interval. @throws std::logic_error The timer was moved from. */
    [[nodiscard]] auto duration() const -> std::chrono::milliseconds;

   private:
    friend class Widget;
    friend class EventLoop;
    friend class Application;
    struct State {
        std::chrono::milliseconds interval;
        std::function<EventResponse()> action;
        std::optional<Clock::time_point> deadline;
        EventLoop* owner = nullptr;
    };

    template <typename Callback>
    static auto checked_callback(Callback action) -> Callback
    {
        if constexpr (requires { static_cast<bool>(action); }) {
            if (!static_cast<bool>(action)) {
                throw std::invalid_argument{"Timer requires a callback"};
            }
        }
        return action;
    }
    static auto widget_action(Widget&, std::function<EventResponse(Widget&)>)
        -> std::function<EventResponse()>;
    void attach(Widget&);
    std::shared_ptr<State> state_;
};
}  // namespace ox
