#ifndef CPPURSES_DEMOS_PALETTE_COLOR_DEFINITION_SETTER_HPP
#define CPPURSES_DEMOS_PALETTE_COLOR_DEFINITION_SETTER_HPP
#include <cctype>
#include <string>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/confirm_button.hpp>
#include <cppurses/widget/widgets/line_edit.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

#include "color_control.hpp"
#include "color_display.hpp"
#include "export_panel.hpp"

namespace palette {

/// Provides interface and implementation of setting a specific color value.
class Color_definition_setter : public cppurses::layout::Vertical<> {
    cppurses::Color current_color_{cppurses::Color::White};

    Color_display& color_display_{
        this->make_child<Color_display>(current_color_)};

    Color_control& color_control_{this->make_child<Color_control>()};

    Export_panel& export_panel_{this->make_child<Export_panel>()};

   public:
    Color_definition_setter();

    /// Change the color you are setting the definition of.
    void change_current_color(cppurses::Color color);

    /// Return the color you are setting the definition of.
    auto current_color() const -> cppurses::Color { return current_color_; }
};

}  // namespace palette
#endif  // CPPURSES_DEMOS_PALETTE_COLOR_DEFINITION_SETTER_HPP
