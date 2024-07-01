#pragma once

#include <ox/widgets2/widget.hpp>

namespace ox {

struct Label : Widget {
    std::string text = "HELLO";

    void set_text(std::string new_text) { text = std::move(new_text); }

    void paint(Canvas c) const override { Painter{c}[{0, 0}] << text; };
};

}  // namespace ox