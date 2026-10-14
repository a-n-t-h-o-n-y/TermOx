#include <ox/button.hpp>

#include <algorithm>
#include <cmath>
#include <utility>

#include <ox/controls.hpp>
#include <ox/text_view.hpp>

namespace ox {
Button::Options const Button::init = {};

Button::Button(Options x)
    : Widget{x.focus_policy, x.width_policy, x.height_policy},
      label{std::move(x.label)},
      pressed_style{x.pressed_style},
      focused_style{x.focused_style},
      decoration{std::move(x.decoration)}
{}

Button::Button(Button&& other)
    : Widget{std::move(other)},
      label{std::move(other.label)},
      pressed_style{std::move(other.pressed_style)},
      focused_style{std::move(other.focused_style)},
      decoration{std::move(other.decoration)},
      on_activate{std::move(other.on_activate)},
      fade_percent{std::exchange(other.fade_percent, decltype(other.fade_percent){})},
      on_press{std::move(other.on_press)},
      on_release{std::move(other.on_release)},
      pressed_{std::exchange(other.pressed_, decltype(other.pressed_){})},
      hovered_{std::exchange(other.hovered_, decltype(other.hovered_){})},
      armed_{std::exchange(other.armed_, decltype(other.armed_){})},
      gesture_{std::exchange(other.gesture_, decltype(other.gesture_){})},
      measure_layout_{std::move(other.measure_layout_)},
      timer_{std::move(other.timer_)}
{ other.timer_.reset(); }
auto Button::operator=(Button&& other) -> Button&
{
    if (this == &other) { return *this; }
    Widget::operator=(std::move(other));
    label = std::move(other.label);
    pressed_style = std::move(other.pressed_style);
    focused_style = std::move(other.focused_style);
    decoration = std::move(other.decoration);
    on_activate = std::move(other.on_activate);
    fade_percent = std::exchange(other.fade_percent, decltype(other.fade_percent){});
    on_press = std::move(other.on_press);
    on_release = std::move(other.on_release);
    pressed_ = std::exchange(other.pressed_, decltype(other.pressed_){});
    hovered_ = std::exchange(other.hovered_, decltype(other.hovered_){});
    armed_ = std::exchange(other.armed_, decltype(other.armed_){});
    gesture_ = std::exchange(other.gesture_, decltype(other.gesture_){});
    measure_layout_ = std::move(other.measure_layout_);
    timer_ = std::move(other.timer_);
    other.timer_.reset();
    return *this;
}
auto Button::paint(Canvas canvas) -> Cursor
{
    auto style = appearance(label.style);
    if (is_focused()) { style = style | focused_style; }
    if (pressed_) { style = style | pressed_style; }
    paint_label(canvas, label.text, label.align, style, label.vertical);
    if (auto* paint = std::get_if<PaintFn>(&decoration)) {
        if (*paint) { (*paint)(canvas); }
    }
    else if (auto const& fade = std::get<Fade>(decoration); fade.paint_fn) {
        fade.paint_fn(canvas, fade_percent);
    }
    return {};
}

auto Button::measure(Area available, esc::WidthPolicy policy) const -> Area
{
    validate(available);
    return measure_label(measure_layout_, label.text, label.vertical, policy);
}
auto Button::activate() -> bool
{
    if (!is_enabled()) { return false; }
    request_repaint();
    on_activate();
    return true;
}
auto Button::handle(KeyEvent const& event) -> EventResponse
{
    if (!activates(event)) { return {}; }
    auto const changed = activate();
    return {.handled = true, .repaint = changed};
}
auto Button::handle(MousePress const& event) -> EventResponse
{
    if (!is_enabled() || !activates(event)) { return {}; }
    armed_ = pressed_ = true;
    ++gesture_;
    capture_pointer();
    request_repaint();
    on_press();
    return {.handled = true, .repaint = true};
}
auto Button::handle(MouseMove const& event) -> EventResponse
{
    if (!armed_) { return {}; }
    auto const changed = pressed_ != contains({{}, size()}, event.at);
    pressed_ = contains({{}, size()}, event.at);
    return {.handled = true, .repaint = changed};
}
auto Button::handle(MouseRelease const& event) -> EventResponse
{
    if (!releases(event) || !armed_) { return {}; }
    auto const fire = is_enabled() && contains({{}, size()}, event.at);
    armed_ = pressed_ = false;
    auto const revision = ++gesture_;
    release_pointer();
    request_repaint();
    auto lifetime = track(*this);
    on_release();
    if (auto* button = lifetime.try_get();
        fire && button && button->gesture_ == revision) {
        button->activate();
    }
    return {.handled = true, .repaint = true};
}
auto Button::pointer_cancel() -> EventResponse
{
    auto const changed = armed_ || pressed_;
    armed_ = pressed_ = false;
    ++gesture_;
    release_pointer();
    return {.repaint = changed};
}

auto Button::focus_in() -> EventResponse
{
    animate();
    return {.handled = true, .repaint = true};
}
auto Button::focus_out() -> EventResponse
{
    pointer_cancel();
    animate();
    return {.handled = true, .repaint = true};
}
auto Button::mouse_enter() -> EventResponse
{
    hovered_ = true;
    animate();
    return {.handled = true, .repaint = true};
}
auto Button::mouse_leave() -> EventResponse
{
    hovered_ = false;
    pressed_ = false;
    animate();
    return {.handled = true, .repaint = true};
}

void Button::animate()
{
    auto const* fade = std::get_if<Fade>(&decoration);
    if (!fade) {
        if (timer_) { timer_->stop(); }
        return;
    }
    auto const selected = hovered_ || is_focused();
    auto const target = selected ? 1.f : 0.f;
    auto const duration = selected ? fade->fade_in : fade->fade_out;
    if (duration.count() <= 0) { fade_percent = target; }
    if (fade_percent == target) {
        if (timer_) { timer_->stop(); }
        return;
    }
    if (!timer_) {
        timer_.emplace(*this, std::chrono::milliseconds{timer_period_ms},
                       [](Button& button) { return button.tick(); });
    }
    if (!timer_->is_running()) { timer_->start(); }
}

auto Button::tick() -> EventResponse
{
    auto const* fade = std::get_if<Fade>(&decoration);
    if (!fade) {
        if (timer_) { timer_->stop(); }
        return {};
    }
    auto const before = fade_percent;
    auto const selected = hovered_ || is_focused();
    auto const duration = selected ? fade->fade_in : fade->fade_out;
    auto const delta =
        duration.count() > 0 ? timer_period_ms / (float)duration.count() : 1.f;
    fade_percent = std::clamp(fade_percent + (selected ? delta : -delta), 0.f, 1.f);
    if (fade_percent == (selected ? 1.f : 0.f) && timer_) { timer_->stop(); }
    return {.handled = true, .repaint = before != fade_percent};
}

auto gradient_blend(RGB one, RGB two, float percent) -> RGB
{
    auto blend = [](std::uint8_t a, std::uint8_t b, float t) {
        return (std::uint8_t)std::clamp(std::lround(a + t * (b - a)), 0L, 255L);
    };

    return RGB{
        blend(one.red, two.red, percent),
        blend(one.green, two.green, percent),
        blend(one.blue, two.blue, percent),
    };
}

}  // namespace ox