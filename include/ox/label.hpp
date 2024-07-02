#pragma once

#include <ox/widget.hpp>

namespace ox {

struct Label : Widget {
    std::string text = "HELLO";

    void set_text(std::string new_text) { text = std::move(new_text); }

    void paint(Canvas c) const override { Painter{c}[{0, 0}] << text; };
};

// /**
//  * A single line horizontal text label.
//  *
//  * @details The text will be truncated if it is too long to fit in the width of the
//  * Label.
//  */
// struct Label {
//     std::string text;
//     enum class Align { Left, Center, Right } align;
//     Brush brush;
// };

// struct LabelInit {
//     std::string text;
//     Label::Align align = Label::Align::Center;
//     Brush brush = {};
// };

// [[nodiscard]] auto label(LabelInit x) -> Label;

// void paint(Label const&, Canvas);

}  // namespace ox