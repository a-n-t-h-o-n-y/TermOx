#include <algorithm>
#include <cmath>
#include <ox/application.hpp>
#include <ox/motion.hpp>
#include <stdexcept>
namespace ox {
namespace {
void finite(double v)
{
    if (!std::isfinite(v)) throw std::invalid_argument{"Nonfinite animation value"};
}
auto curve(double t, Ease ease) -> double
{
    switch (ease) {
        case Ease::Linear: return t;
        case Ease::OutCubic: return 1 - std::pow(1 - t, 3);
        case Ease::InOutCubic:
            return t < .5 ? 4 * t * t * t : 1 - std::pow(-2 * t + 2, 3) / 2;
        // Critically damped response, normalized to reach the endpoint at duration.
        case Ease::Spring:
            return (1 - (1 + 8 * t) * std::exp(-8 * t)) / (1 - 9 * std::exp(-8.));
    }
    throw std::invalid_argument{"Invalid easing"};
}
}  // namespace
Tween::Tween(double v) : value_{v}, from_{v}, target_{v} { finite(v); }
void Tween::reset(double v)
{
    finite(v);
    value_ = from_ = target_ = v;
    active_ = false;
}
void Tween::to(double v, Transition t, Clock::time_point now)
{
    finite(v);
    if (t.duration.count() < 0)
        throw std::invalid_argument{"Negative transition duration"};
    (void)curve(0, t.ease);
    sample(now);
    from_ = value_;
    target_ = v;
    transition_ = t;
    started_ = now;
    active_ = value_ != target_ && t.duration.count() > 0;
    if (!active_) value_ = target_;
}
auto Tween::sample(Clock::time_point now) -> bool
{
    if (!active_) return false;
    auto before = value_;
    auto t =
        std::clamp(std::chrono::duration<double, std::milli>{now - started_}.count() /
                       (double)transition_.duration.count(),
                   0., 1.);
    value_ = std::lerp(from_, target_, curve(t, transition_.ease));
    if (t == 1) {
        value_ = target_;
        active_ = false;
    }
    return value_ != before;
}
Animation::Animation(Widget& widget, double v) : state_{std::make_shared<State>(v)}
{
    auto lifetime = track(widget);
    state_->eligible = [lifetime] {
        auto* w = lifetime.try_get();
        // Application synchronizes inherited visibility/input state before ticking.
        return w && w->visible && w->is_enabled();
    };
    state_->policy = [lifetime] {
        auto* w = lifetime.try_get();
        return w ? w->theme().motion : MotionPolicy{.enabled = false};
    };
    state_->repaint = [lifetime] {
        if (auto* w = lifetime.try_get()) w->request_repaint();
    };
    std::erase_if(widget.animations_, [](auto const& w) { return w.expired(); });
    widget.animations_.push_back(state_);
    if (widget.application_) widget.application_->loop().watch(*this);
}
auto Animation::value() const -> double
{
    return state_ ? (state_->policy().enabled ? state_->tween.value()
                                              : state_->tween.target())
                  : 0;
}
auto Animation::target() const -> double { return state_ ? state_->tween.target() : 0; }
auto Animation::active() const -> bool { return state_ && state_->tween.active(); }
void Animation::reset(double v)
{
    if (!state_) throw std::logic_error{"Moved-from animation"};
    auto changed = state_->tween.value() != v;
    state_->tween.reset(v);
    if (changed) state_->repaint();
}
void Animation::to(double v, Transition t, Clock::time_point now)
{
    if (!state_) throw std::logic_error{"Moved-from animation"};
    if (t.duration.count() < 0)
        throw std::invalid_argument{"Negative transition duration"};
    if (!state_->policy().enabled || !state_->eligible())
        t.duration = std::chrono::milliseconds{0};
    state_->tween.to(v, t, now);
    state_->repaint();
}
auto Animation::advance(std::shared_ptr<State> const& state, Clock::time_point now)
    -> EventResponse
{
    if (!state || !state->tween.active()) return {};
    if (!state->eligible()) {
        state->tween.reset(state->tween.target());
        return {};
    }
    auto changed = false;
    if (!state->policy().enabled) {
        changed = state->tween.value() != state->tween.target();
        state->tween.reset(state->tween.target());
    }
    else
        changed = state->tween.sample(now);
    auto result = EventResponse{.repaint = changed};
    if (changed && state->callback) {
        auto callback = state->callback;
        result = combine(result, callback(state->tween.value()));
    }
    return result;
}
auto Animation::advance(Clock::time_point now) -> EventResponse
{
    auto state = state_;
    return advance(state, now);
}
void Animation::on_frame(std::function<EventResponse(double)> callback)
{
    if (!state_) throw std::logic_error{"Moved-from animation"};
    state_->callback = std::move(callback);
}
}  // namespace ox
