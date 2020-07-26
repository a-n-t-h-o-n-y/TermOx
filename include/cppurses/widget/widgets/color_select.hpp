#ifndef CPPURSES_WIDGET_WIDGETS_COLOR_SELECT_HPP
#define CPPURSES_WIDGET_WIDGETS_COLOR_SELECT_HPP
#include <array>

#include <signals/signals.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widgets/button.hpp>

namespace cppurses {

/// A single line of colored buttons.
template <std::size_t Color_count>
class Color_bar : public layout::Horizontal<Button> {
   public:
    sig::Signal<void(Color)> color_selected;

   public:
    explicit Color_bar(std::array<Color, Color_count> colors)
    {
        using namespace pipe;
        for (auto c : colors) {
            this->make_child() | bg(c) | on_mouse_press([this, c](auto& m) {
                if (m.button == Mouse::Button::Left)
                    this->color_selected(c);
            });
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
        using namespace pipe;
        *this | children() |
            on_color_selected([this](auto c) { this->color_selected(c); });
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
