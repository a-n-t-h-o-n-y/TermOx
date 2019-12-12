#ifndef CPPURSES_WIDGET_WIDGETS_COLOR_SELECT_HPP
#define CPPURSES_WIDGET_WIDGETS_COLOR_SELECT_HPP
#include <initializer_list>

#include <signals/signals.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace cppurses {

struct Color_bar : layout::Horizontal<Push_button> {
    sig::Signal<void(Color)> color_selected;

    Color_bar(std::initializer_list<Color> colors)
    {
        for (auto c : colors) {
            auto& btn = this->make_child();
            btn.brush.set_background(c);
            btn.clicked.connect([this, c] { this->color_selected(c); });
        }
    }
};

/// Provides a visual display of the 16 colors and sends a Signal on clicks.
class Color_select : public layout::Vertical<Color_bar> {
   public:
    /// Emitted on left mouse button press, sends along the Color clicked on.
    sig::Signal<void(Color)> color_selected;

    Color_select()
    {
        row_one_.color_selected.connect(
            [this](Color c) { this->color_selected(c); });
        row_two_.color_selected.connect(
            [this](Color c) { this->color_selected(c); });
    }

   private:
    using Init_list = std::initializer_list<Color>;
    Color_bar& row_one_{this->make_child(Init_list{
        Color::Black, Color::Dark_red, Color::Dark_blue, Color::Dark_gray,
        Color::Brown, Color::Green, Color::Red, Color::Gray})};
    Color_bar& row_two_{this->make_child(Init_list{
        Color::Blue, Color::Orange, Color::Light_gray, Color::Light_green,
        Color::Violet, Color::Light_blue, Color::Yellow, Color::White})};
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_COLOR_SELECT_HPP
