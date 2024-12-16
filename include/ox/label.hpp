#pragma once

#include <cstdint>
#include <string>

#include <ox/core/core.hpp>
#include <ox/widget.hpp>

namespace ox {

/**
 * A single line horizontal text label.
 */
class Label : public Widget {
   public:
    std::string text;
    enum class Align : std::uint8_t { Left, Center, Right } align;
    Brush brush;

   public:
    struct Init {
        std::string text = "";
        Align align = Align::Center;
        Brush brush = {};
    } static const init;

    Label(Init state = init);

   public:
    void paint(Canvas c) override;
};

}  // namespace ox