#pragma once

#include <chrono>
#include <deque>
#include <mutex>
#include <optional>
#include <vector>

#include <ox/core/dispatch.hpp>
#include <ox/core/terminal.hpp>
#include <ox/timer.hpp>

namespace ox {

/** Owns events, timer scheduling, and frame requests independently of terminal I/O.
 * post() is thread safe. Dispatch, timers, and painting run on the owning thread.
 */
class EventLoop {
   public:
    using Clock = std::chrono::steady_clock;

    EventLoop();
    ~EventLoop();

    /** Wait for posted events, timers, or a borrowed terminal wait source.
     * Does not consume terminal input. Call on the dispatch thread. */
    void wait(esc::WaitState input = {});

    Frame screen{{0, 0}};
    Cursor cursor;

    void post(Event event);
    void watch(Timer const& timer);
    void unwatch(Timer const& timer);
    /** Apply effects at the dispatch boundary; first quit request wins. */
    void apply(EventResponse response);
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
            auto response = dispatch(event, handler, screen);
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
        if constexpr (requires {
                          { handler.paint(Canvas{screen}) } -> std::same_as<Cursor>;
                      }) {
            screen.clear();
            cursor = handler.paint(Canvas{screen});
        }
        return true;
    }

   private:
    friend class Application;
    void watch(std::weak_ptr<Timer::State> timer);
    void unwatch(std::weak_ptr<Timer::State> timer);

    esc::WaitSource wake_;
    std::intptr_t wake_write_ = 0;
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
                                  std::function<EventResponse()> prepare = {}) -> int
{
    loop.post(esc::Resize{terminal.size()});
    while (!loop.exit_code()) {
        if (prepare) { loop.apply(prepare()); }
        loop.step(handler);
        if (loop.paint(handler)) { terminal.commit_changes(loop.screen, loop.cursor); }
        if (loop.exit_code()) { break; }
        loop.wait(terminal.wait_state());
        if (auto event = terminal.read(std::chrono::milliseconds{0})) {
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
