#include <ox/button.hpp>

#include <algorithm>
#include <functional>
#include <utility>
#include <variant>

#include <ox/core/common.hpp>
#include <ox/put.hpp>

namespace ox {

BasicButton::BasicButton() : Widget{FocusPolicy::Strong, SizePolicy::flex()} {}

void BasicButton::key_press(Key k)
{
    if (k == Key::Enter) { this->on_press(); }
}

auto BasicButton::mouse_press(Mouse m) -> void
{
    if (m.button == Mouse::Button::Left) { this->on_press(); }
}

auto BasicButton::mouse_release(Mouse m) -> void
{
    if (m.button == Mouse::Button::Left) { this->on_release(); }
}

// -------------------------------------------------------------------------------------

Button::Options const Button::init = {};

Button::Button(Options x)
    : label{std::move(x.label)},
      pressed_mod{std::move(x.pressed_mod)},
      focused_mod{std::move(x.focused_mod)},
      decoration_{[d = std::move(x.decoration)] {
          return std::visit(
              Overload{
                  [](PaintFn fn) -> DecorationInternal { return std::move(fn); },
                  [](Fade f) -> DecorationInternal {
                      return {FadeInternal{.fade = std::move(f)}};
                  },
              },
              std::move(d));
      }()}
{}

void Button::paint(Canvas c)
{
    auto temp_label = Label{label};

    // Label Modifications
    if (in_focus_ && focused_mod) { focused_mod(temp_label); }
    if (pressed_ && pressed_mod) { pressed_mod(temp_label); }

    temp_label.paint(c);

    // Decoration over Canvas
    std::visit(Overload{
                   [&c](PaintFn const& fn) {
                       if (fn) { fn(c); }
                   },
                   [&c](FadeInternal const& f) {
                       if (f.fade.paint_fn) { f.fade.paint_fn(c, f.percent); }
                   },
               },
               decoration_);
}

void Button::mouse_press(Mouse m)
{
    if (m.button == Mouse::Button::Left) { pressed_ = true; }
    this->BasicButton::mouse_press(m);
}

void Button::mouse_release(Mouse m)
{
    if (m.button == Mouse::Button::Left) { pressed_ = false; }
    this->BasicButton::mouse_release(m);
}

void Button::focus_in()
{
    in_focus_ = true;
    this->start_select();
}

void Button::focus_out()
{
    in_focus_ = false;
    this->end_select();
}

void Button::mouse_enter() { this->start_select(); }

void Button::mouse_leave() { this->end_select(); }

void Button::timer(int id)
{
    auto const update_fade = [&id](FadeInternal& f) {
        if (id != f.timer.id()) { return; }

        auto const delta =
            timer_period_ms / (float)(f.direction == +1 ? f.fade.fade_in.count()
                                                        : f.fade.fade_out.count());

        f.percent = std::clamp(0.f, f.percent + delta * (float)f.direction, 1.f);
        if (f.percent == 0.f || f.percent == 1.f) { f.timer.stop(); }
    };

    std::visit(Overload{[](PaintFn const&) {}, update_fade}, decoration_);
}

void Button::start_select()
{
    std::visit(Overload{
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
    std::visit(Overload{
                   [](PaintFn const&) {},
                   [](FadeInternal& f) {
                       f.direction = -1;
                       if (not f.timer.is_running()) { f.timer.start(); }
                   },
               },
               decoration_);
}

// -------------------------------------------------------------------------------------

auto gradient_blend(TrueColor one, TrueColor two, float percent) -> TrueColor
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