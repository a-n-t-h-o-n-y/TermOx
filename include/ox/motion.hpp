#pragma once
#include <chrono>
#include <functional>
#include <memory>
#include <ox/core/events.hpp>
#include <ox/theme.hpp>
namespace ox {
class Widget;
class EventLoop;
enum class Ease { Linear, OutCubic, InOutCubic, Spring };
struct Transition {
    std::chrono::milliseconds duration{160};
    Ease ease = Ease::OutCubic;
};
/** An elapsed-time animation value. Retargets from its sampled current position. */
class Tween {
   public:
    using Clock = std::chrono::steady_clock;
    explicit Tween(double value = 0);
    void reset(double);
    void to(double, Transition = {}, Clock::time_point now = Clock::now());
    auto sample(Clock::time_point now = Clock::now()) -> bool;
    auto value() const -> double { return value_; }
    auto target() const -> double { return target_; }
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
    using Clock = Tween::Clock;
    Animation(Widget&, double value = 0);
    Animation(Animation const&) = delete;
    Animation(Animation&&) = default;
    auto operator=(Animation const&) -> Animation& = delete;
    auto operator=(Animation&&) -> Animation& = default;
    auto value() const -> double;
    auto target() const -> double;
    auto active() const -> bool;
    void reset(double);
    void to(double, Transition = {}, Clock::time_point now = Clock::now());
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
