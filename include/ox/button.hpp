#pragma once

#include <optional>
#include <string>

#include <signals_light/signal.hpp>

#include <ox/core/core.hpp>
#include <ox/label.hpp>
#include <ox/widget.hpp>

namespace ox {

class Button : public Widget {
   public:
    Label label;

    Brush brush;
    Brush on_press_brush;
    Brush on_hover_brush;

    sl::Signal<void()> on_press;
    sl::Signal<void()> on_release;

   public:
    Button(std::string text = "",
           Brush brush = {},
           std::optional<Brush> on_press_brush = std::nullopt,
           std::optional<Brush> on_hover_brush = std::nullopt)
        : label{text, Label::Align::Center, brush},
          brush{brush},
          on_press_brush{on_press_brush.value_or(brush)},
          on_hover_brush{on_hover_brush.value_or(brush)}
    {}

   public:
    auto paint(Canvas c) -> void override { label.paint(c); }

    auto mouse_press(Mouse m) -> void override
    {
        if (m.button == Mouse::Button::Left) {
            label.brush = on_press_brush;
            on_press();
        }
    }

    auto mouse_release(Mouse m) -> void override
    {
        if (m.button == Mouse::Button::Left) {
            label.brush = on_hover_brush;
            on_release();
        }
    }

    auto mouse_enter() -> void override { label.brush = on_hover_brush; }

    auto mouse_leave() -> void override { label.brush = brush; }
};

}  // namespace ox