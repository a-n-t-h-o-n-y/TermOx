#ifndef TERMOX_DEMOS_PALETTE_COLOR_DISPLAY_PANEL_HPP
#define TERMOX_DEMOS_PALETTE_COLOR_DISPLAY_PANEL_HPP
#include <termox/painter/color.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/widgets/color_select.hpp>

namespace palette {

/// Displays every light and medium shade of a particular base color.
class Shade_display : public ox::Widget {
   public:
    explicit Shade_display(ox::Color base) : base_{base} {}

    /// Paint the shades from the top to the bottom.
    void invert() { inverted_ = true; }

   protected:
    bool paint_event() override;

   private:
    ox::Color base_;
    bool inverted_{false};
};

/// Top bank of shades to display, the first 8 colors.
struct Top_shades : ox::layout::Horizontal<Shade_display> {
    Shade_display& black_shade{this->make_child(ox::Color::Black)};
    Shade_display& dark_red_shade{this->make_child(ox::Color::Dark_red)};
    Shade_display& dark_blue_shade{this->make_child(ox::Color::Dark_blue)};
    Shade_display& dark_gray_shade{this->make_child(ox::Color::Dark_gray)};
    Shade_display& brown_shade{this->make_child(ox::Color::Brown)};
    Shade_display& green_shade{this->make_child(ox::Color::Green)};
    Shade_display& red_shade{this->make_child(ox::Color::Red)};
    Shade_display& gray_shade{this->make_child(ox::Color::Gray)};
};

/// Bottom bank of shades to display, the last 8 colors.
struct Bottom_shades : ox::layout::Horizontal<Shade_display> {
    Bottom_shades();

    Shade_display& blue_shade{this->make_child(ox::Color::Blue)};
    Shade_display& orange_shade{this->make_child(ox::Color::Orange)};
    Shade_display& light_gray_shade{this->make_child(ox::Color::Light_gray)};
    Shade_display& light_green_shade{this->make_child(ox::Color::Light_green)};
    Shade_display& violet_shade{this->make_child(ox::Color::Violet)};
    Shade_display& light_blue_shade{this->make_child(ox::Color::Light_blue)};
    Shade_display& yellow_shade{this->make_child(ox::Color::Yellow)};
    Shade_display& white_shade{this->make_child(ox::Color::White)};
};

/// Holds both the pure colors and the various shades.
struct All_colors_display : ox::layout::Vertical<> {
    All_colors_display();

    Top_shades& top_shades         = make_child<Top_shades>();
    ox::Color_select& color_select = make_child<ox::Color_select>();
    Bottom_shades& bottom_shades   = make_child<Bottom_shades>();
};

}  // namespace palette
#endif  // TERMOX_DEMOS_PALETTE_COLOR_DISPLAY_PANEL_HPP
