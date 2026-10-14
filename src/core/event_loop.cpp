#include <ox/core/event_loop.hpp>

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <system_error>

#ifdef _WIN32
#    define NOMINMAX
#    include <windows.h>
#else
#    include <fcntl.h>
#    include <poll.h>
#    include <unistd.h>
#    include <cerrno>
#endif

namespace ox {

EventLoop::EventLoop()
{
#ifdef _WIN32
    auto handle = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!handle) {
        throw std::system_error{(int)GetLastError(), std::system_category(),
                                "Create event-loop wakeup"};
    }
    wake_ = {esc::WaitSourceKind::WindowsHandle, (std::intptr_t)handle};
#else
    int descriptors[2];
    if (::pipe(descriptors) == -1) {
        throw std::system_error{errno, std::generic_category(),
                                "Create event-loop pipe"};
    }
    for (int descriptor : descriptors) {
        if (::fcntl(descriptor, F_SETFL, O_NONBLOCK) == -1 ||
            ::fcntl(descriptor, F_SETFD, FD_CLOEXEC) == -1) {
            auto const error = errno;
            ::close(descriptors[0]);
            ::close(descriptors[1]);
            throw std::system_error{error, std::generic_category(),
                                    "Configure event-loop pipe"};
        }
    }
    wake_ = {esc::WaitSourceKind::FileDescriptor, descriptors[0]};
    wake_write_ = descriptors[1];
#endif
}

EventLoop::~EventLoop()
{
#ifdef _WIN32
    CloseHandle((HANDLE)wake_.value);
#else
    ::close((int)wake_.value);
    ::close((int)wake_write_);
#endif
    for (auto const& entry : timers_) {
        if (auto timer = entry.lock(); timer && timer->owner == this) {
            timer->owner = nullptr;
        }
    }
}

void EventLoop::post(Event event)
{
    auto const lock = std::scoped_lock{mutex_};
    auto const notify = events_.empty();
    events_.push_back(std::move(event));
    if (!notify) { return; }
#ifdef _WIN32
    if (!SetEvent((HANDLE)wake_.value)) {
        throw std::system_error{(int)GetLastError(), std::system_category(),
                                "Wake event loop"};
    }
#else
    char byte = 0;
    while (::write((int)wake_write_, &byte, 1) == -1) {
        if (errno == EINTR) { continue; }
        if (errno == EAGAIN) { break; }
        throw std::system_error{errno, std::generic_category(), "Wake event loop"};
    }
#endif
}

auto EventLoop::take_events() -> std::deque<Event>
{
    auto events = std::deque<Event>{};
    auto const lock = std::scoped_lock{mutex_};
    if (events_.empty()) { return events; }
    events.swap(events_);
    // Reset under the queue lock: a later empty-to-nonempty post must wake again.
#ifdef _WIN32
    if (!ResetEvent((HANDLE)wake_.value)) {
        throw std::system_error{(int)GetLastError(), std::system_category(),
                                "Reset event-loop wakeup"};
    }
#else
    char byte;
    while (::read((int)wake_.value, &byte, 1) == -1 && errno == EINTR) {}
#endif
    return events;
}

void EventLoop::wait(esc::WaitState input)
{
    if (input.ready) { return; }
    while (true) {
        auto const now = Clock::now();
        auto timeout = wait_duration(now);
        if (input.deadline) {
            timeout =
                std::min(timeout, std::max(std::chrono::milliseconds{0},
                                           std::chrono::ceil<std::chrono::milliseconds>(
                                               *input.deadline - now)));
        }
        if (timeout == std::chrono::milliseconds{0}) { return; }
#ifdef _WIN32
        HANDLE handles[] = {(HANDLE)wake_.value, (HANDLE)input.source.value};
        auto const count = input.source.kind == esc::WaitSourceKind::None ? 1UL : 2UL;
        if (count == 2 && input.source.kind != esc::WaitSourceKind::WindowsHandle) {
            throw std::invalid_argument{"Event loop requires a Windows wait handle"};
        }
        auto const duration =
            timeout == std::chrono::milliseconds::max()
                ? INFINITE
                : (DWORD)std::min(timeout.count(),
                                  (decltype(timeout.count()))(INFINITE - 1));
        auto const result = WaitForMultipleObjects(count, handles, FALSE, duration);
        if (result == WAIT_FAILED) {
            throw std::system_error{(int)GetLastError(), std::system_category(),
                                    "Wait for event loop"};
        }
        return;
#else
        pollfd descriptors[] = {{(int)wake_.value, POLLIN, 0},
                                {(int)input.source.value, POLLIN, 0}};
        auto const count = input.source.kind == esc::WaitSourceKind::None ? 1UL : 2UL;
        if (count == 2 && input.source.kind != esc::WaitSourceKind::FileDescriptor) {
            throw std::invalid_argument{"Event loop requires a file descriptor"};
        }
        auto const duration =
            timeout == std::chrono::milliseconds::max()
                ? -1
                : (int)std::min(
                      timeout.count(),
                      (decltype(timeout.count()))std::numeric_limits<int>::max());
        auto const result = ::poll(descriptors, (nfds_t)count, duration);
        if (result == -1) {
            if (errno == EINTR) { continue; }
            throw std::system_error{errno, std::generic_category(),
                                    "Wait for event loop"};
        }
        for (auto const& descriptor : descriptors) {
            if (descriptor.revents & POLLNVAL) {
                throw std::invalid_argument{"Invalid event-loop wait descriptor"};
            }
        }
        return;
#endif
    }
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

void EventLoop::request_repaint() { apply({.repaint = true}); }

void EventLoop::quit(int code) { apply({.quit = code}); }

void EventLoop::apply(EventResponse response)
{
    repaint_ |= response.repaint;
    if (response.quit && !exit_code_) { exit_code_ = response.quit; }
}

void EventLoop::merge(EventResponse& result, EventResponse response)
{
    apply(response);
    result = combine(result, response);
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
