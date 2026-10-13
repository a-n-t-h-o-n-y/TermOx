#include <ox/core/event_loop.hpp>

#include <algorithm>
#include <stdexcept>

namespace ox {

EventLoop::~EventLoop()
{
    for (auto const& entry : timers_) {
        if (auto timer = entry.lock(); timer && timer->owner == this) {
            timer->owner = nullptr;
        }
    }
}

void EventLoop::post(Event event)
{
    auto const lock = std::scoped_lock{mutex_};
    events_.push_back(std::move(event));
}

auto EventLoop::take_events() -> std::deque<Event>
{
    auto events = std::deque<Event>{};
    auto const lock = std::scoped_lock{mutex_};
    events.swap(events_);
    return events;
}

void EventLoop::watch(Timer const& timer) { this->watch(timer.state_); }

void EventLoop::watch(std::weak_ptr<Timer::State> timer)
{
    auto state = timer.lock();
    if (!state) { return; }
    if (state->owner && state->owner != this) {
        throw std::logic_error{"Timer already belongs to an event loop"};
    }
    if (std::ranges::none_of(
            timers_, [&](auto const& entry) { return entry.lock() == state; })) {
        timers_.push_back(std::move(timer));
    }
    state->owner = this;
}

void EventLoop::unwatch(Timer const& timer) { this->unwatch(timer.state_); }

void EventLoop::unwatch(std::weak_ptr<Timer::State> entry)
{
    if (auto timer = entry.lock(); timer && timer->owner == this) {
        timer->deadline.reset();
        timer->owner = nullptr;
    }
}

void EventLoop::request_repaint() { repaint_ = true; }

void EventLoop::quit(int code) { exit_code_ = code; }

void EventLoop::merge(EventResponse& result, EventResponse response)
{
    result.handled |= response.handled;
    result.repaint |= response.repaint;
    repaint_ |= response.repaint;
    if (response.quit && !exit_code_) { exit_code_ = response.quit; }
    result.quit = exit_code_;
}

auto EventLoop::wait_duration(Clock::time_point now) const -> std::chrono::milliseconds
{
    {
        auto const lock = std::scoped_lock{mutex_};
        if (!events_.empty()) { return std::chrono::milliseconds{0}; }
    }
    if (repaint_ || exit_code_) { return std::chrono::milliseconds{0}; }
    auto wait = std::chrono::milliseconds::max();
    for (auto const& entry : timers_) {
        auto timer = entry.lock();
        if (!timer || timer->owner != this || !timer->deadline) { continue; }
        auto const remaining =
            std::chrono::ceil<std::chrono::milliseconds>(*timer->deadline - now);
        wait = std::min(wait, std::max(remaining, std::chrono::milliseconds{0}));
    }
    return wait;
}
}  // namespace ox
