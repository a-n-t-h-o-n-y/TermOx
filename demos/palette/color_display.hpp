#ifndef TERMOX_DEMOS_PALETTE_COLOR_DISPLAY_HPP
#define TERMOX_DEMOS_PALETTE_COLOR_DISPLAY_HPP
#include <termox/painter/color.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/widgets/text_display.hpp>

namespace palette {

/// Square visual display of a given Color.
struct Color_patch : ox::Widget {
    explicit Color_patch(ox::Color value)
    {
        this->set_color(value);
        this->width_policy.fixed(5);
        this->height_policy.fixed(2);
    }

    void set_color(ox::Color value)
    {
        this->brush.background = value;
        this->update();
    }
};

/// Title and name display of a given Color.
struct Color_name : ox::layout::Vertical<ox::Text_display> {
    ox::Text_display& title = this->make_child("Color:");
    ox::Text_display& name  = this->make_child("White");
};

/// Displays a text name and a square visual of a given Color.
struct Color_display : ox::layout::Horizontal<> {
    explicit Color_display(ox::Color color)
    {
        this->height_policy.fixed(3);
        this->border.enable();
        this->border.segments.disable_all();
        this->border.segments.south.enable();
        this->set_color(color);
    }

    /// Change the currently displayed color.
    void set_color(ox::Color color)
    {
        text.name.set_contents(color_to_string(color));
        square.set_color(color);
    }

    Color_name& text    = this->make_child<Color_name>();
    Color_patch& square = this->make_child<Color_patch>(ox::Color::White);
};
}  // namespace palette
#endif  // TERMOX_DEMOS_PALETTE_COLOR_DISPLAY_HPP
