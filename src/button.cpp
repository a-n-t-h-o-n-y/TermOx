#include <ox/button.hpp>

#include <functional>
#include <utility>

namespace ox {

Button::Options const Button::init = {};

Button::Button(Options x)
    : Widget{FocusPolicy::None, SizePolicy::flex()},
      label{{
          .text = std::move(x.text),
          .align = Align::Center,
          .brush = x.brush,
      }},
      brush{x.brush},
      on_press_brush{std::move(x.on_press_brush)},
      on_hover_brush{std::move(x.on_hover_brush)}
{}

auto Button::paint(Canvas c) -> void { label.paint(c); }

auto Button::mouse_press(Mouse m) -> void
{
    if (m.button == Mouse::Button::Left) {
        label.brush = on_press_brush(brush);
        on_press();
    }
}

auto Button::mouse_release(Mouse m) -> void
{
    if (m.button == Mouse::Button::Left) {
        label.brush = on_hover_brush(brush);
        on_release();
    }
}

auto Button::mouse_enter() -> void { label.brush = on_hover_brush(brush); }

auto Button::mouse_leave() -> void { label.brush = brush; }

}  // namespace ox