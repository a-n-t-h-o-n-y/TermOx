#pragma once

#include <ox/widgets2/widget.hpp>

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

}  // namespace ox