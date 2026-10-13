#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <optional>

#include <ox/core/events.hpp>

namespace ox {
class Widget;
class EventLoop;
class Application;

/** A movable timer handle. The owning event loop executes its callback. */
class Timer {
   public:
    using Clock = std::chrono::steady_clock;

    Timer(std::chrono::milliseconds interval,
          std::function<EventResponse()> action,
          bool launch = true);

    explicit Timer(Widget& widget,
                   std::chrono::milliseconds interval,
                   bool launch = false);

    Timer(Timer const&) = delete;
    Timer(Timer&&) = default;
    auto operator=(Timer const&) -> Timer& = delete;
    auto operator=(Timer&&) -> Timer& = default;

    void start(Clock::time_point now = Clock::now());
    void stop();
    [[nodiscard]] auto is_running() const -> bool;
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

    std::shared_ptr<State> state_;
};
}  // namespace ox
