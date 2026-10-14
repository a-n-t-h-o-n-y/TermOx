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

auto Timer::widget_action(Widget& widget, std::function<EventResponse(Widget&)> action)
    -> std::function<EventResponse()>
{
    return [target = track(widget), action = std::move(action)] {
        auto* widget = target.try_get();
        if (!widget || !widget->visible || !widget->enabled) { return EventResponse{}; }
        if (widget->application_ && !widget->application_->is_interactive(*widget)) {
            return EventResponse{};
        }
        return action(*widget);
    };
}

void Timer::attach(Widget& widget)
{
    std::erase_if(widget.timers_, [](auto const& entry) { return entry.expired(); });
    widget.timers_.push_back(state_);
    if (widget.application_) { widget.application_->loop().watch(*this); }
}

void Timer::start(Clock::time_point now)
{
    if (!state_) { throw std::logic_error{"Moved-from timer"}; }
    state_->deadline = now + state_->interval;
}
void Timer::stop()
{
    if (state_) { state_->deadline.reset(); }
}
auto Timer::is_running() const -> bool
{ return state_ && state_->deadline.has_value(); }
auto Timer::duration() const -> std::chrono::milliseconds
{
    if (!state_) { throw std::logic_error{"Moved-from timer"}; }
    return state_->interval;
}
}  // namespace ox
