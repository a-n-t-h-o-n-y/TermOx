#pragma once

#include <chrono>
#include <deque>
#include <mutex>
#include <optional>
#include <vector>

#include <ox/core/terminal.hpp>
#include <ox/timer.hpp>

namespace ox {

/** Owns events, timer scheduling, and frame requests independently of terminal I/O.
 * post() is thread safe. Dispatch, timers, and painting run on the owning thread.
 */
class EventLoop {
   public:
    using Clock = std::chrono::steady_clock;

    ~EventLoop();

    ScreenBuffer screen{{0, 0}};
    Terminal::Cursor cursor;

    void post(Event event);
    void watch(Timer const& timer);
    void unwatch(Timer const& timer);
    void request_repaint();
    void quit(int code = 0);

    [[nodiscard]] auto exit_code() const -> std::optional<int> { return exit_code_; }
    [[nodiscard]] auto needs_repaint() const -> bool { return repaint_; }
    [[nodiscard]] auto wait_duration(Clock::time_point now = Clock::now()) const
        -> std::chrono::milliseconds;

    template <typename Handler>
    auto step(Handler& handler, Clock::time_point now = Clock::now()) -> EventResponse
    {
        auto result = EventResponse{.quit = exit_code_};
        if (exit_code_) { return result; }
        auto events = take_events();
        for (auto const& event : events) {
            auto response = apply_event(event, handler, screen);
            merge(result, response);
            if (exit_code_) { return result; }
        }

        std::erase_if(timers_, [this](auto const& entry) {
            auto timer = entry.lock();
            return !timer || timer->owner != this;
        });
        auto const count = timers_.size();
        for (auto i = std::size_t{0}; i < count; ++i) {
            auto timer = timers_[i].lock();
            if (!timer || timer->owner != this || !timer->deadline ||
                *timer->deadline > now) {
                continue;
            }
            // Deliver one tick after a delay, preserving cadence without a catch-up
            // burst.
            auto const elapsed = now - *timer->deadline;
            *timer->deadline += (elapsed / timer->interval + 1) * timer->interval;
            merge(result, timer->action());
            if (exit_code_) { break; }
        }
        return result;
    }

    template <typename Handler>
    auto paint(Handler& handler) -> bool
    {
        if (!repaint_ || exit_code_) { return false; }
        repaint_ = false;
        if constexpr (HandlesPaint<Handler>) {
            screen.fill(Glyph{});
            cursor = handler.handle_paint(Canvas{screen, {0, 0}, screen.size()});
        }
        return true;
    }

   private:
    friend class Application;
    void watch(std::weak_ptr<Timer::State> timer);
    void unwatch(std::weak_ptr<Timer::State> timer);

    mutable std::mutex mutex_;
    std::deque<Event> events_;
    std::vector<std::weak_ptr<Timer::State>> timers_;
    bool repaint_ = false;
    std::optional<int> exit_code_;

    auto take_events() -> std::deque<Event>;
    void merge(EventResponse& result, EventResponse response);
};

/** Drive terminal input, dispatch, and rendering on one thread. */
template <typename Handler>
[[nodiscard]] auto process_events(EventLoop& loop,
                                  Terminal& terminal,
                                  Handler& handler,
                                  std::function<void()> prepare = {}) -> int
{
    loop.post(esc::Resize{terminal.size()});
    while (!loop.exit_code()) {
        if (prepare) { prepare(); }
        loop.step(handler);
        if (loop.paint(handler)) { terminal.commit_changes(loop.screen, loop.cursor); }
        if (loop.exit_code()) { break; }
        // Bound the wait so events posted by worker threads remain responsive.
        auto const timeout =
            std::min(loop.wait_duration(), std::chrono::milliseconds{16});
        if (auto event = terminal.read(timeout)) {
            loop.post(std::move(*event));
            // Batch immediately available input; cap the batch so timers cannot starve.
            for (int i = 0; i < 63; ++i) {
                auto next = terminal.read(std::chrono::milliseconds{0});
                if (!next) { break; }
                loop.post(std::move(*next));
            }
        }
    }
    return *loop.exit_code();
}
}  // namespace ox
