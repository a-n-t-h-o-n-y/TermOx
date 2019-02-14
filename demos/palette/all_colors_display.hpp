#ifndef CPPURSES_DEMOS_PALETTE_COLOR_DISPLAY_PANEL_HPP
#define CPPURSES_DEMOS_PALETTE_COLOR_DISPLAY_PANEL_HPP
#include <cppurses/painter/color.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/color_select.hpp>

namespace palette {

/// Displays every light and medium shade of a particular base color.
class Shade_display : public cppurses::Widget {
   public:
    explicit Shade_display(cppurses::Color base);

    /// Paint the shades from the top to the bottom.
    void invert();

   protected:
    bool paint_event() override;

   private:
    cppurses::Color base_;
    bool inverted_{false};
};

/// Top bank of shades to display, the first 8 colors.
struct Top_shades : cppurses::layout::Horizontal {
    Shade_display& black_shade{
        this->make_child<Shade_display>(cppurses::Color::Black)};
    Shade_display& dark_red_shade{
        this->make_child<Shade_display>(cppurses::Color::Dark_red)};
    Shade_display& dark_blue_shade{
        this->make_child<Shade_display>(cppurses::Color::Dark_blue)};
    Shade_display& dark_gray_shade{
        this->make_child<Shade_display>(cppurses::Color::Dark_gray)};
    Shade_display& brown_shade{
        this->make_child<Shade_display>(cppurses::Color::Brown)};
    Shade_display& green_shade{
        this->make_child<Shade_display>(cppurses::Color::Green)};
    Shade_display& red_shade{
        this->make_child<Shade_display>(cppurses::Color::Red)};
    Shade_display& gray_shade{
        this->make_child<Shade_display>(cppurses::Color::Gray)};
};

/// Bottom bank of shades to display, the last 8 colors.
struct Bottom_shades : cppurses::layout::Horizontal {
    Bottom_shades();

    Shade_display& blue_shade{
        this->make_child<Shade_display>(cppurses::Color::Blue)};
    Shade_display& orange_shade{
        this->make_child<Shade_display>(cppurses::Color::Orange)};
    Shade_display& light_gray_shade{
        this->make_child<Shade_display>(cppurses::Color::Light_gray)};
    Shade_display& light_green_shade{
        this->make_child<Shade_display>(cppurses::Color::Light_green)};
    Shade_display& violet_shade{
        this->make_child<Shade_display>(cppurses::Color::Violet)};
    Shade_display& light_blue_shade{
        this->make_child<Shade_display>(cppurses::Color::Light_blue)};
    Shade_display& yellow_shade{
        this->make_child<Shade_display>(cppurses::Color::Yellow)};
    Shade_display& white_shade{
        this->make_child<Shade_display>(cppurses::Color::White)};
};

/// Holds both the pure colors and the various shades.
struct All_colors_display : cppurses::layout::Vertical {
    All_colors_display();

    Top_shades& top_shades{this->make_child<Top_shades>()};
    cppurses::Color_select& color_select{
        this->make_child<cppurses::Color_select>()};
    Bottom_shades& bottom_shades{this->make_child<Bottom_shades>()};
};

}  // namespace palette
#endif  // CPPURSES_DEMOS_PALETTE_COLOR_DISPLAY_PANEL_HPP
