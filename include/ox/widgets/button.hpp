#pragma once

#include <optional>

#include <signals_light/signal.hpp>

#include <ox/widgets/label.hpp>

namespace ox::widgets {

struct Button {
    Label label;

    Brush brush;
    Brush on_press_brush;
    Brush on_hover_brush;

    sl::Signal<void()> on_press;
    sl::Signal<void()> on_release;
};

struct ButtonParameters {
    std::string text;
    Brush brush = {};
    std::optional<Brush> on_press_brush = std::nullopt;
    std::optional<Brush> on_hover_brush = std::nullopt;
    sl::Slot<void()> on_press = [] {};
    sl::Slot<void()> on_release = [] {};
};

[[nodiscard]] inline auto button(ButtonParameters p) -> Button
{
    return {
        .label =
            {
                .text = std::move(p.text),
                .align = Label::Align::Center,
                .brush = p.brush,
            },
        .brush = p.brush,
        .on_press_brush = p.on_press_brush.value_or(p.brush),
        .on_hover_brush = p.on_hover_brush.value_or(p.brush),
        .on_press = std::move(p.on_press),
        .on_release = std::move(p.on_release),
    };
}

inline auto paint(Button const& b, Canvas c) -> void { paint(b.label, c); }

inline auto mouse_press(Button& b, Mouse m) -> void
{
    if (m.button == Mouse::Button::Left) {
        b.label.brush = b.on_press_brush;
        b.on_press();
    }
}

inline auto mouse_release(Button& b, Mouse m) -> void
{
    if (m.button == Mouse::Button::Left) {
        b.label.brush = b.on_hover_brush;
        b.on_release();
    }
}

inline auto mouse_enter(Button& b) -> void { b.label.brush = b.on_hover_brush; }

inline auto mouse_leave(Button& b) -> void { b.label.brush = b.brush; }

}  // namespace ox::widgets