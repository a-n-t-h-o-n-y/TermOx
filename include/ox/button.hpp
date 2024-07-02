#pragma once

#include <ox/widget.hpp>

namespace ox {

struct Button : Widget {
    sl::Signal<void(char, int)> on_press;

    // TODO use a label or take text as a parameter
    Button() : Widget{FocusPolicy::Strong} {}

    void mouse_press(Mouse m) override
    {
        if (m.button == Mouse::Button::Left) {
            this->on_press('c', 5);
        }
    }

    // void mouse_enter() override { cursor = Cursor::Arrow; }

    // void mouse_leave() override { cursor = std::nullopt; }

    void paint(Canvas c) const override
    {
        auto const width = size.width;
        auto const height = size.height;
        auto const text = std::string{"Press me"};
        auto const text_width = text.size();
        int const text_x = (width - text_width) / 2;
        int const text_y = height / 2;
        Painter{c}[{text_x, text_y}] << text;
    }
};

// struct Button {
//     Label label;

//     Brush brush;
//     Brush on_press_brush;
//     Brush on_hover_brush;

//     sl::Signal<void()> on_press;
//     sl::Signal<void()> on_release;
// };

// struct ButtonInit {
//     std::string text;
//     Brush brush = {};
//     std::optional<Brush> on_press_brush = std::nullopt;
//     std::optional<Brush> on_hover_brush = std::nullopt;
//     sl::Slot<void()> on_press = [] {};
//     sl::Slot<void()> on_release = [] {};
// };

// // TODO create more of these, different look and feels so client call site is
// simpler.
// [[nodiscard]] inline auto button(ButtonInit x) -> Button
// {
//     return {
//         .label =
//             {
//                 .text = std::move(x.text),
//                 .align = Label::Align::Center,
//                 .brush = x.brush,
//             },
//         .brush = x.brush,
//         .on_press_brush = x.on_press_brush.value_or(x.brush),
//         .on_hover_brush = x.on_hover_brush.value_or(x.brush),
//         .on_press = std::move(x.on_press),
//         .on_release = std::move(x.on_release),
//     };
// }

// inline auto paint(Button const& b, Canvas c) -> void { paint(b.label, c); }

// inline auto mouse_press(Button& b, Mouse m) -> void
// {
//     if (m.button == Mouse::Button::Left) {
//         b.label.brush = b.on_press_brush;
//         b.on_press();
//     }
// }

// inline auto mouse_release(Button& b, Mouse m) -> void
// {
//     if (m.button == Mouse::Button::Left) {
//         b.label.brush = b.on_hover_brush;
//         b.on_release();
//     }
// }

// inline auto mouse_enter(Button& b) -> void { b.label.brush = b.on_hover_brush; }

// inline auto mouse_leave(Button& b) -> void { b.label.brush = b.brush; }

}  // namespace ox