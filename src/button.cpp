#include <ox/button.hpp>

#include <algorithm>
#include <functional>
#include <utility>
#include <variant>

#include <zzz/overload.hpp>

#include <ox/core/common.hpp>
#include <ox/put.hpp>

namespace ox {

BasicButton::BasicButton(FocusPolicy fp, SizePolicy sp) : Widget{fp, sp} {}

auto BasicButton::key_press(KeyEvent k) -> EventResponse
{
    if (!k.matches(KeyCode::Enter, {}, k.action)) { return {}; }
    this->on_press();

    return {.handled = true, .repaint = true};
}

auto BasicButton::mouse_press(MousePress m) -> EventResponse
{
    if (m.button != MouseButton::Left) { return {}; }
    this->on_press();

    return {.handled = true, .repaint = true};
}

auto BasicButton::mouse_release(MouseRelease m) -> EventResponse
{
    if (m.button && m.button != MouseButton::Left) { return {}; }
    this->on_release();

    return {.handled = true, .repaint = true};
}

// -------------------------------------------------------------------------------------

Button::Options const Button::init = {};

Button::Button(Options x)
    : BasicButton{x.focus_policy, x.size_policy},
      label{std::move(x.label)},
      press_mod{std::move(x.press_mod)},
      focus_mod{std::move(x.focus_mod)},
      decoration_{[this, d = std::move(x.decoration)] {
          return std::visit(
              zzz::Overload{
                  [](PaintFn fn) -> DecorationInternal { return fn; },
                  [this](Fade f) -> DecorationInternal {
                      return {FadeInternal{
                          .fade = std::move(f),
                          .timer =
                              Timer{
                                  *this,
                                  std::chrono::milliseconds{timer_period_ms},
                                  false,
                              },
                      }};
                  },
              },
              std::move(d));
      }()}
{}

void Button::paint(Canvas c)
{
    auto temp_label = Label{label};

    // Label Modifications
    if (in_focus_ && focus_mod) { focus_mod(temp_label); }
    if (pressed_ && press_mod) { press_mod(temp_label); }

    temp_label.paint(c);

    // Decoration over Canvas
    std::visit(zzz::Overload{
                   [&c](PaintFn const& fn) {
                       if (fn) { fn(c); }
                   },
                   [&c](FadeInternal const& f) {
                       if (f.fade.paint_fn) { f.fade.paint_fn(c, f.percent); }
                   },
               },
               decoration_);
}

auto Button::mouse_press(MousePress m) -> EventResponse
{
    if (m.button == MouseButton::Left) { pressed_ = true; }
    return this->BasicButton::mouse_press(m);
}

auto Button::mouse_release(MouseRelease m) -> EventResponse
{
    if (!m.button || m.button == MouseButton::Left) { pressed_ = false; }
    return this->BasicButton::mouse_release(m);
}

auto Button::focus_in() -> EventResponse
{
    in_focus_ = true;
    this->start_select();

    return {.handled = true, .repaint = true};
}

auto Button::focus_out() -> EventResponse
{
    in_focus_ = false;
    this->end_select();

    return {.handled = true, .repaint = true};
}

auto Button::mouse_enter() -> EventResponse
{
    this->start_select();
    return {.handled = true};
}

auto Button::mouse_leave() -> EventResponse
{
    this->end_select();
    return {.handled = true};
}

auto Button::timer() -> EventResponse
{
    auto fade = std::get_if<FadeInternal>(&decoration_);
    if (!fade) { return {}; }
    auto const before = fade->percent;
    auto const duration =
        fade->direction == +1 ? fade->fade.fade_in : fade->fade.fade_out;
    auto const delta =
        duration.count() > 0 ? timer_period_ms / (float)duration.count() : 1.f;
    fade->percent =
        std::clamp(fade->percent + delta * (float)fade->direction, 0.f, 1.f);
    if (fade->percent == 0.f || fade->percent == 1.f) { fade->timer.stop(); }
    return {.handled = true, .repaint = before != fade->percent};
}

void Button::start_select()
{
    std::visit(zzz::Overload{
                   [](PaintFn const&) {},
                   [](FadeInternal& f) {
                       f.direction = +1;
                       if (not f.timer.is_running()) { f.timer.start(); }
                   },
               },
               decoration_);
}

void Button::end_select()
{
    std::visit(zzz::Overload{
                   [](PaintFn const&) {},
                   [](FadeInternal& f) {
                       f.direction = -1;
                       if (not f.timer.is_running()) { f.timer.start(); }
                   },
               },
               decoration_);
}

// -------------------------------------------------------------------------------------

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