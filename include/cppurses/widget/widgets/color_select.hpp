#ifndef CPPURSES_WIDGET_WIDGETS_COLOR_SELECT_HPP
#define CPPURSES_WIDGET_WIDGETS_COLOR_SELECT_HPP
#include <memory>
#include <string>
#include <utility>

#include <signals_light/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widgets/button.hpp>

namespace cppurses {

class Color_tile : public Button {
   public:
    Color_tile(Color c, bool display_number)
        : color_{std::to_wstring(c.value) | fg(Color::Foreground)},
          number_{display_number}
    {
        using namespace cppurses::pipe;
        *this | bg(c) | fg(Color::Foreground);
    }

   protected:
    auto paint_event() -> bool override
    {
        if (number_)
            Painter{*this}.put(color_, {0uL, 0uL});
        return Button::paint_event();
    }

   private:
    Glyph_string color_;
    bool number_;
};

using Color_line = layout::Horizontal<Color_tile>;

/// Displays each color of the current palette.
/** Updates when Terminal::set_pallete() succeeds. */
class Color_select : public layout::Vertical<Color_line> {
   public:
    sl::Signal<void(Color)> color_selected;

   public:
    explicit Color_select(bool display_numbers = false)
        : numbers_{display_numbers}
    {
        System::terminal.palette_changed.connect(
            [this](auto const& pal) { this->set_palette(pal); });
    }

   private:
    void set_palette(Color_palette const& pal)
    {
        this->delete_all_children();
        auto const size           = pal.size();
        auto constexpr row_length = 8uL;
        auto count                = 0uL;

        while (count != size) {
            auto& color_line = this->make_child();
            for (auto i = 0uL; i < row_length && count != size; ++i, ++count) {
                using namespace cppurses::pipe;
                auto const color = pal[count].color;
                color_line.make_child(color, numbers_) |
                    on_press([this, color]() { color_selected(color); });
            }
        }
    }

   private:
    bool numbers_;
};

/// Helper function to create an instance.
template <typename... Args>
auto color_select(Args&&... args) -> std::unique_ptr<Color_select>
{
    return std::make_unique<Color_select>(std::forward<Args>(args)...);
}

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_COLOR_SELECT_HPP
