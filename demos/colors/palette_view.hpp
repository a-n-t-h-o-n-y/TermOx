#ifndef CPPURSES_DEMOS_COLORS_COLORS_DISPLAY_HPP
#define CPPURSES_DEMOS_COLORS_COLORS_DISPLAY_HPP
#include <cmath>
#include <string>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widget.hpp>

namespace colors {

class Color_tile : public cppurses::Widget {
   public:
    Color_tile(cppurses::Color c) : c_{c} { *this | cppurses::pipe::bg(c); }

   protected:
    auto paint_event() -> bool override
    {
        cppurses::Painter{*this}.put(std::to_string(c_.value), 0, 0);
        return Widget::paint_event();
    }

   private:
    cppurses::Color c_;
};

class Color_line : public cppurses::layout::Horizontal<Color_tile> {
   public:
    template <typename... Colors>
    Color_line(Colors... colors)
    {
        (this->make_child(colors), ...);
    }
};

/// Displays each color of a Palette.
class Palette_view : public cppurses::layout::Vertical<Color_line> {
   public:
    Palette_view(cppurses::ANSI_palette const& palette)
        : is_ansi_{true}, ansi_palette_{palette}
    {
        this->focus_policy = cppurses::Focus_policy::Strong;

        auto const size       = palette.size();
        auto const row_length = std::floor(std::sqrt(size));
        auto count            = 0uL;
        while (true) {
            auto& color_line = this->make_child();
            for (auto i = 0uL; i < row_length; ++i, ++count) {
                if (count == size)
                    break;
                color_line.make_child(palette[count].color);
            }
            if (count == size)
                break;
        }
    }

    Palette_view(cppurses::True_color_palette const& palette)
        : is_ansi_{false}, tc_palette_{palette}
    {
        this->focus_policy = cppurses::Focus_policy::Strong;

        auto const size       = palette.size();
        auto const row_length = std::floor(std::sqrt(size));
        auto count            = 0uL;
        while (true) {
            auto& color_line = this->make_child();
            for (auto i = 0uL; i < row_length; ++i, ++count) {
                if (count == size)
                    break;
                color_line.make_child(palette[count].ansi_def.color);
            }
            if (count == size)
                break;
        }
    }

   protected:
    auto focus_in_event() -> bool override
    {
        using namespace cppurses;
        if (is_ansi_)
            System::terminal.set_palette(ansi_palette_);
        else
            System::terminal.set_palette(tc_palette_);
        return cppurses::layout::Vertical<Color_line>::focus_in_event();
    }

   private:
    bool is_ansi_;
    cppurses::ANSI_palette ansi_palette_;
    cppurses::True_color_palette tc_palette_;
};

}  // namespace colors
#endif  // CPPURSES_DEMOS_COLORS_COLORS_DISPLAY_HPP
