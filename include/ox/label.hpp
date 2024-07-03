#pragma once

#include <ox/core/core.hpp>
#include <ox/widget.hpp>

namespace ox {

/**
 * A single line horizontal text label.
 *
 * @details The text will be truncated if it is too long to fit across the Label.
 */
struct Label : Widget {
    std::string text;
    enum class Align { Left, Center, Right } align;
    Brush brush;

    void paint(Canvas c) const override;
};

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