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
    using Clock = std::chrono::steady_clock;

    Timer(std::chrono::milliseconds interval,
          std::function<EventResponse()> action,
          bool launch = true);

    template <typename W, typename Callback>
        requires std::derived_from<W, Widget> &&
                 std::is_invocable_r_v<EventResponse, Callback&, W&>
    Timer(W& widget,
          std::chrono::milliseconds interval,
          Callback action,
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
