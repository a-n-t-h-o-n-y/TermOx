#ifndef CPPURSES_DEMOS_COLORS_COLORS_DISPLAY_HPP
#define CPPURSES_DEMOS_COLORS_COLORS_DISPLAY_HPP
#include <signals/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/painter/palette/basic.hpp>
#include <cppurses/painter/palette/basic8.hpp>
#include <cppurses/painter/palette/dawn_bringer16.hpp>
#include <cppurses/painter/palette/dawn_bringer32.hpp>
#include <cppurses/painter/palette/en4.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/terminal/terminal.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/button.hpp>
#include <cppurses/widget/widgets/cycle_box.hpp>

namespace colors {

struct Color_tile : cppurses::Button {
    explicit Color_tile(cppurses::Color c) { *this | cppurses::pipe::bg(c); }
};

using Color_line = cppurses::layout::Horizontal<Color_tile>;

/// Displays each color of the current palette.
/** Updates when Terminal::set_pallete() succeeds. */
class Palette_view : public cppurses::layout::Vertical<Color_line> {
   public:
    sig::Signal<void(cppurses::Color)> color_selected;

   public:
    Palette_view()
    {
        cppurses::System::terminal.palette_changed.connect(
            [this](auto const& pal) { this->set_palette(pal); });
    }

   private:
    void set_palette(cppurses::Color_palette const& pal)
    {
        this->Layout::clear();
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

class Palette_picker : public cppurses::Labeled_cycle_box {
   public:
    sig::Signal<void(cppurses::Color_palette)> palette_picked;

   public:
    Palette_picker() : Labeled_cycle_box{"Palette"}
    {
        this->cycle_box.add_option("Dawn Bringer 16").connect([this]() {
            palette_picked(cppurses::dawn_bringer16::palette);
        });
        this->cycle_box.add_option("Dawn Bringer 32").connect([this]() {
            palette_picked(cppurses::dawn_bringer32::palette);
        });
        this->cycle_box.add_option("Basic 16").connect([this]() {
            palette_picked(cppurses::basic::palette);
        });
        this->cycle_box.add_option("Basic 8").connect(
            [this]() { palette_picked(cppurses::basic8::palette); });
        this->cycle_box.add_option("EN 4").connect(
            [this]() { palette_picked(cppurses::en4::palette); });
    }
};

class Palette_demo : public cppurses::layout::Vertical<> {
   public:
    Palette_view& palette_view     = this->make_child<Palette_view>();
    Palette_picker& palette_picker = this->make_child<Palette_picker>();

   public:
    Palette_demo()
    {
        using namespace cppurses::pipe;
        *this | strong_focus();
        palette_picker.palette_picked.connect([](auto const& pal) {
            cppurses::System::terminal.set_palette(pal);
        });
    }
};

}  // namespace colors
#endif  // CPPURSES_DEMOS_COLORS_COLORS_DISPLAY_HPP
