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
    Label(std::string text_ = "", Align align_ = Align::Center, Brush brush_ = {})
        : text{std::move(text_)}, align{align_}, brush{brush_}
    {}

   public:
    void paint(Canvas c) override;
};

}  // namespace ox