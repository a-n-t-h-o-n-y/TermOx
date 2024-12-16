#include <ox/button.hpp>

#include <utility>

namespace ox {

Button::Init const Button::init = {};

Button::Button(Init state)
    : Widget{FocusPolicy::None, SizePolicy::flex()},
      label{{
          .text = std::move(state.text),
          .align = Label::Align::Center,
          .brush = state.brush,
      }},
      brush{state.brush},
      on_press_brush{state.on_press_brush.value_or(state.brush)},
      on_hover_brush{state.on_hover_brush.value_or(state.brush)} {};

auto Button::paint(Canvas c) -> void { label.paint(c); }

auto Button::mouse_press(Mouse m) -> void
{
    if (m.button == Mouse::Button::Left) {
        label.brush = on_press_brush;
        on_press();
    }
}

auto Button::mouse_release(Mouse m) -> void
{
    if (m.button == Mouse::Button::Left) {
        label.brush = on_hover_brush;
        on_release();
    }
}

auto Button::mouse_enter() -> void { label.brush = on_hover_brush; }

auto Button::mouse_leave() -> void { label.brush = brush; }

}  // namespace ox