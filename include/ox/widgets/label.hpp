#pragma once

#include <string>

#include <ox/core.hpp>

namespace ox {

/**
 * A simple text display widget.
 * @details The text will be centered vertically, and can be aligned left, center, or
 * right horizontally. The text will be truncated if it is too long to fit in the width
 * of the Label in a single line.
 */
struct Label {
    std::string text;
    enum class Align { Left, Center, Right } align = Align::Center;
    Brush brush = {};
};

void paint(Label const&, Canvas);

}  // namespace ox