#pragma once

#include <signals_light/signal.hpp>

#include <termox/widgets/label.hpp>

namespace ox::widgets {

struct Button {
    Label label;
    sl::Signal<void()> clicked;
};

inline auto paint(Button const& b, Canvas c) -> void { paint(b.label, c); }

inline auto mouse_press(Button& b, Mouse m) -> void
{
    if (m.button == Mouse::Button::Left) {
        b.clicked();
    }
}

}  // namespace ox::widgets