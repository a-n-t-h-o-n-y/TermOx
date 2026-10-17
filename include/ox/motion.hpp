/** @file
 * Elapsed-time interpolation and widget-bound animation schedules.
 */
#pragma once
#include <chrono>
#include <functional>
#include <memory>
#include <ox/core/events.hpp>
#include <ox/theme.hpp>
namespace ox {
class Widget;
class EventLoop;
/** Interpolation curves; Spring is a normalized critically damped curve with no overshoot. */
enum class Ease {
    Linear, ///< Advance at a constant rate.
    OutCubic, ///< Decelerate with a cubic curve.
    InOutCubic, ///< Accelerate and then decelerate with a cubic curve.
    Spring ///< Approach the endpoint with a normalized critically damped curve.
};
/** Duration and easing for one scalar transition. */
struct Transition {
    /** Nonnegative transition length; zero assigns the target immediately. */
    std::chrono::milliseconds duration{160};
    /** Interpolation curve applied to normalized elapsed time. */
    Ease ease = Ease::OutCubic;
};
/** An elapsed-time animation value. Retargets from its sampled current position. */
class Tween {
   public:
    /** Monotonic clock used to make sampling independent of wall-clock changes. */
    using Clock = std::chrono::steady_clock;
    /** Create a settled finite value. @throws std::invalid_argument The value is nonfinite. */
    explicit Tween(double value = 0);
    /** Assign a finite value and target immediately, cancelling interpolation. */
    void reset(double);
    /** Retarget from the value sampled at now; equal targets or zero duration settle immediately.
     * @throws std::invalid_argument Nonfinite target, negative duration, or invalid easing.
     */
    void to(double, Transition = {}, Clock::time_point now = Clock::now());
    /** Advance to elapsed time, clamp to the endpoints, and return whether the sampled
     * value changed.
     */
    auto sample(Clock::time_point now = Clock::now()) -> bool;
    /** Return the last sampled value without advancing time. */
    auto value() const -> double { return value_; }
    /** Return the requested endpoint, whether active or settled. */
    auto target() const -> double { return target_; }
    /** Return whether another sample may change the current value. */
    auto active() const -> bool { return active_; }

   private:
    double value_, from_, target_;
    Transition transition_;
    Clock::time_point started_;
    bool active_ = false;
};
/** Move-aware widget presentation, advanced by the event loop's shared frame clock.
 * Destruction cancels work. Hidden widgets are not scheduled; reattachment
 * samples elapsed time. Disabled widgets settle without repainting. */
class Animation {
   public:
    /** Clock shared with the event loop's animation scheduler. */
    using Clock = Tween::Clock;
    /** Bind a retained animation to a borrowed widget identity; the initial value must
     * be finite.
     */
    Animation(Widget&, double value = 0);
    /** An animation schedule has one owning handle. */
    Animation(Animation const&) = delete;
    /** Transfer the registered animation state without restarting it. */
    Animation(Animation&&) = default;
    /** Animation schedules cannot be copy-assigned. */
    auto operator=(Animation const&) -> Animation& = delete;
    /** Release this animation and take the source's registered state. */
    auto operator=(Animation&&) -> Animation& = default;
    /** Return the sampled presentation value, or the target when motion is disabled;
     * moved-from handles return zero.
     */
    auto value() const -> double;
    /** Return the endpoint; moved-from handles return zero. */
    auto target() const -> double;
    /** Return whether the retained tween is active; moved-from handles return false. */
    auto active() const -> bool;
    /** Assign a finite value and target immediately, cancelling interpolation. */
    void reset(double);
    /** Retarget from the value sampled at now; equal targets or zero duration settle immediately.
     * @throws std::invalid_argument Nonfinite target, negative duration, or invalid easing.
     */
    void to(double, Transition = {}, Clock::time_point now = Clock::now());
    /** Sample one frame and return repaint/callback effects; ineligible widgets settle
     * without invoking frame callbacks.
     */
    auto advance(Clock::time_point now = Clock::now()) -> EventResponse;
    /** Optional frame effects. Capture a tracked widget, just as for a signal slot. */
    void on_frame(std::function<EventResponse(double)>);

   private:
    friend class Widget;
    friend class EventLoop;
    friend class Application;
    struct State {
        explicit State(double value) : tween{value} {}
        Tween tween;
        EventLoop* owner = nullptr;
        std::function<bool()> eligible;
        std::function<MotionPolicy()> policy;
        std::function<void()> repaint;
        std::function<EventResponse(double)> callback;
    };
    static auto advance(std::shared_ptr<State> const&, Clock::time_point)
        -> EventResponse;
    std::shared_ptr<State> state_;
};
}  // namespace ox
