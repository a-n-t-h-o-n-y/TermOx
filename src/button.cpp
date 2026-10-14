#include <ox/button.hpp>
#include <ox/put.hpp>

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
{
    feedback_.on_frame([lifetime = track(*this)](double value) {
        if (auto* self = lifetime.try_get()) self->fade_percent = (float)value;
        return EventResponse{.repaint = true};
    });
}

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
      feedback_{std::move(other.feedback_)}
{}
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
    feedback_ = std::move(other.feedback_);
    return *this;
}
auto Button::paint(Canvas canvas) -> Cursor
{
    auto const& visual = theme();
    auto style = appearance(label.style, Surface::Raised);
    if (visual.styled) {
        auto amount = feedback_.active() ? feedback_.value()
                                         : (hovered_ || is_focused() ? 1. : 0.);
        if (label.style.background == Color{default_color})
            style.background =
                mix_color(visual.palette.surface, visual.palette.selection, amount);
        if (label.style.foreground == Color{default_color})
            style.foreground =
                mix_color(visual.palette.foreground, visual.palette.accent, amount);
        if (pressed_) {
            style.background = visual.palette.accent;
            style.foreground = visual.palette.background;
        }
    }
    if (is_focused()) { style = style | focused_style; }
    if (pressed_) { style = style | pressed_style; }
    if (!is_enabled()) style = style | disabled_style;
    if (visual.styled) {
        fill(canvas, style);
        auto inset = std::min(2, canvas.size().width);
        paint_label(
            canvas.subcanvas({inset, 0}, {std::max(0, canvas.size().width - 2 * inset),
                                          canvas.size().height}),
            label.text, label.align, style, label.vertical);
        if (is_focused() && canvas.size().width > 1 && canvas.size().height > 0) {
            auto y = canvas.size().height / 2;
            canvas.write({0, y}, visual.ascii ? U"[" : U"╺", style);
            canvas.write({canvas.size().width - 1, y}, visual.ascii ? U"]" : U"╸",
                         style);
        }
    }
    else
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
    auto result = measure_label(measure_layout_, label.text, label.vertical, policy);
    if (theme().styled) {
        if (result.width > std::numeric_limits<int>::max() - 4)
            throw std::length_error{"Button extent"};
        result.width += 4;
    }
    return result;
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
    if (!fade && !theme().styled) {
        feedback_.reset(0);
        fade_percent = 0;
        return;
    }
    auto selected = hovered_ || is_focused();
    auto duration =
        fade ? (selected ? fade->fade_in : fade->fade_out) : theme().motion.feedback;
    feedback_.to(selected ? 1. : 0., {duration, Ease::OutCubic});
    fade_percent = (float)feedback_.value();
}
auto Button::tick() -> EventResponse { return feedback_.advance(); }

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