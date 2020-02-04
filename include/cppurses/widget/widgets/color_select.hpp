#ifndef CPPURSES_WIDGET_WIDGETS_COLOR_SELECT_HPP
#define CPPURSES_WIDGET_WIDGETS_COLOR_SELECT_HPP
#include <array>

#include <signals/signals.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace cppurses {

template <std::size_t Color_count>
struct Color_bar : layout::Horizontal<Push_button> {
    sig::Signal<void(Color)> color_selected;

    Color_bar(std::array<Color, Color_count> colors)
    {
        for (auto c : colors) {
            auto& btn = this->make_child();
            btn.brush.set_background(c);
            btn.clicked.connect([this, c] { this->color_selected(c); });
        }
    }
};

/// Provides a visual display of the 16 colors and sends a Signal on clicks.
class Color_select : public layout::Vertical<Color_bar<8>> {
   public:
    /// Emitted on left mouse button press, sends along the Color clicked on.
    sig::Signal<void(Color)> color_selected;

   public:
    Color_select()
    {
        row_one_.color_selected.connect(
            [this](Color c) { this->color_selected(c); });
        row_two_.color_selected.connect(
            [this](Color c) { this->color_selected(c); });
    }

   private:
    using Color_list = std::array<Color, 8>;

    Color_bar<8>& row_one_ = this->make_child(Color_list{
        Color::Black, Color::Dark_red, Color::Dark_blue, Color::Dark_gray,
        Color::Brown, Color::Green, Color::Red, Color::Gray});

    Color_bar<8>& row_two_ = this->make_child(Color_list{
        Color::Blue, Color::Orange, Color::Light_gray, Color::Light_green,
        Color::Violet, Color::Light_blue, Color::Yellow, Color::White});
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_COLOR_SELECT_HPP
