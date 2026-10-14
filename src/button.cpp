#include <ox/button.hpp>

#include <algorithm>
#include <cmath>
#include <utility>

#include <ox/controls.hpp>

namespace ox {
Button::Options const Button::init = {};

Button::Button(Options x)
    : Widget{x.focus_policy, x.size_policy},
      label{std::move(x.label)},
      pressed_style{x.pressed_style},
      focused_style{x.focused_style},
      decoration{std::move(x.decoration)}
{}

auto Button::paint(Canvas canvas) -> Cursor
{
    auto style = label.style;
    if (is_focused()) { style = style | focused_style; }
    if (pressed) { style = style | pressed_style; }
    paint_label(canvas, label.text, label.align, style);
    if (auto* paint = std::get_if<PaintFn>(&decoration)) {
        if (*paint) { (*paint)(canvas); }
    }
    else if (auto const& fade = std::get<Fade>(decoration); fade.paint_fn) {
        fade.paint_fn(canvas, fade_percent);
    }
    return {};
}

auto Button::handle(KeyEvent const& event) -> EventResponse
{
    if (activates(event)) {
        pressed = true;
        on_press();
        return {.handled = true, .repaint = true};
    }
    if (!releases(event) || !pressed) { return {}; }
    pressed = false;
    on_release();
    return {.handled = true, .repaint = true};
}
auto Button::handle(MousePress const& event) -> EventResponse
{
    if (!activates(event)) { return {}; }
    pressed = true;
    on_press();
    return {.handled = true, .repaint = true};
}
auto Button::handle(MouseRelease const& event) -> EventResponse
{
    if (!releases(event) || !pressed) { return {}; }
    pressed = false;
    on_release();
    return {.handled = true, .repaint = true};
}

auto Button::focus_in() -> EventResponse
{
    animate();
    return {.handled = true, .repaint = true};
}
auto Button::focus_out() -> EventResponse
{
    pressed = false;
    animate();
    return {.handled = true, .repaint = true};
}
auto Button::mouse_enter() -> EventResponse
{
    hovered = true;
    animate();
    return {.handled = true, .repaint = true};
}
auto Button::mouse_leave() -> EventResponse
{
    hovered = false;
    pressed = false;
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
    auto const selected = hovered || is_focused();
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
    auto const selected = hovered || is_focused();
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