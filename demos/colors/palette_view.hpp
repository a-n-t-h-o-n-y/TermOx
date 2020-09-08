#ifndef CPPURSES_DEMOS_COLORS_COLORS_DISPLAY_HPP
#define CPPURSES_DEMOS_COLORS_COLORS_DISPLAY_HPP
#include <signals/signal.hpp>

#include <cppurses/painter/palette/basic.hpp>
#include <cppurses/painter/palette/basic8.hpp>
#include <cppurses/painter/palette/dawn_bringer16.hpp>
#include <cppurses/painter/palette/dawn_bringer32.hpp>
#include <cppurses/painter/palette/en4.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/terminal/terminal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widgets/color_select.hpp>
#include <cppurses/widget/widgets/cycle_box.hpp>

namespace colors {

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
    cppurses::Color_select& palette_view =
        this->make_child<cppurses::Color_select>();
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
