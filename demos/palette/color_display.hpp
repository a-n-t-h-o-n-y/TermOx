#ifndef CPPURSES_DEMOS_PALETTE_COLOR_DISPLAY_HPP
#define CPPURSES_DEMOS_PALETTE_COLOR_DISPLAY_HPP
#include <cppurses/painter/color.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

namespace palette {

/// Square visual display of a given Color.
struct Color_patch : cppurses::Widget {
    explicit Color_patch(cppurses::Color value)
    {
        this->set_color(value);
        this->width_policy.fixed(5);
        this->height_policy.fixed(2);
    }

    void set_color(cppurses::Color value)
    {
        this->brush.set_background(value);
        this->update();
    }
};

/// Title and name display of a given Color.
struct Color_name : cppurses::layout::Vertical<cppurses::Text_display> {
    cppurses::Text_display& title = this->make_child("Color:");
    cppurses::Text_display& name  = this->make_child("White");
};

/// Displays a text name and a square visual of a given Color.
struct Color_display : cppurses::layout::Horizontal<> {
    explicit Color_display(cppurses::Color color)
    {
        this->height_policy.fixed(3);
        this->border.enable();
        this->border.segments.disable_all();
        this->border.segments.south.enable();
        this->set_color(color);
    }

    /// Change the currently displayed color.
    void set_color(cppurses::Color color)
    {
        text.name.set_contents(color_to_string(color));
        square.set_color(color);
    }

    Color_name& text    = this->make_child<Color_name>();
    Color_patch& square = this->make_child<Color_patch>(cppurses::Color::White);
};
}  // namespace palette
#endif  // CPPURSES_DEMOS_PALETTE_COLOR_DISPLAY_HPP
