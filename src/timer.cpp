#include <ox/timer.hpp>

#include <stdexcept>

#include <ox/application.hpp>
#include <ox/widget.hpp>

namespace ox {

Timer::Timer(std::chrono::milliseconds interval,
             std::function<EventResponse()> action,
             bool launch)
    : state_{std::make_shared<State>(State{interval, std::move(action), {}})}
{
    if (interval.count() <= 0) {
        throw std::invalid_argument{"Timer interval must be positive"};
    }
    if (!state_->action) { throw std::invalid_argument{"Timer requires a callback"}; }
    if (launch) { this->start(); }
}

Timer::Timer(Widget& widget, std::chrono::milliseconds interval, bool launch)
    : Timer{
          interval,
          [target = track(widget)] {
              if (!target.valid()) { return EventResponse{}; }
              auto& widget = target.get();
              if (!widget.active || !widget.enabled) { return EventResponse{}; }
              if (widget.application_ && !widget.application_->is_interactive(widget)) {
                  return EventResponse{};
              }
              return widget.timer();
          },
          launch}
{
    widget.timers_.push_back(state_);
    if (widget.application_) { widget.application_->loop().watch(*this); }
}

void Timer::start(Clock::time_point now) { state_->deadline = now + state_->interval; }
void Timer::stop() { state_->deadline.reset(); }
auto Timer::is_running() const -> bool
{ return state_ && state_->deadline.has_value(); }
auto Timer::duration() const -> std::chrono::milliseconds { return state_->interval; }
}  // namespace ox
