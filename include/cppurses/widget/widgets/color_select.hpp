#ifndef CPPURSES_WIDGET_WIDGETS_COLOR_SELECT_HPP
#define CPPURSES_WIDGET_WIDGETS_COLOR_SELECT_HPP
#include <memory>
#include <utility>

#include <signals/signals.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widgets/button.hpp>

namespace cppurses {

struct Color_tile : cppurses::Button {
    explicit Color_tile(cppurses::Color c) { *this | cppurses::pipe::bg(c); }
};

using Color_line = cppurses::layout::Horizontal<Color_tile>;

/// Displays each color of the current palette.
/** Updates when Terminal::set_pallete() succeeds. */
class Color_select : public cppurses::layout::Vertical<Color_line> {
   public:
    sig::Signal<void(cppurses::Color)> color_selected;

   public:
    Color_select()
    {
        cppurses::System::terminal.palette_changed.connect(
            [this](auto const& pal) { this->set_palette(pal); });
    }

   private:
    void set_palette(cppurses::Color_palette const& pal)
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
                color_line.make_child(color) |
                    on_press([this, color]() { color_selected(color); });
            }
        }
    }
};

/// Helper function to create an instance.
template <typename... Args>
auto color_select(Args&&... args) -> std::unique_ptr<Color_select>
{
    return std::make_unique<Color_select>(std::forward<Args>(args)...);
}

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_COLOR_SELECT_HPP
