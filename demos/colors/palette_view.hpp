#ifndef CPPURSES_DEMOS_COLORS_COLORS_DISPLAY_HPP
#define CPPURSES_DEMOS_COLORS_COLORS_DISPLAY_HPP
#include <utility>

#include <signals/signal.hpp>

#include <cppurses/painter/palette/amstrad_cpc.hpp>
#include <cppurses/painter/palette/apple_ii.hpp>
#include <cppurses/painter/palette/ashes.hpp>
#include <cppurses/painter/palette/basic.hpp>
#include <cppurses/painter/palette/basic8.hpp>
#include <cppurses/painter/palette/cga.hpp>
#include <cppurses/painter/palette/commodore_64.hpp>
#include <cppurses/painter/palette/commodore_vic20.hpp>
#include <cppurses/painter/palette/dawn_bringer16.hpp>
#include <cppurses/painter/palette/dawn_bringer32.hpp>
#include <cppurses/painter/palette/en4.hpp>
#include <cppurses/painter/palette/gameboy.hpp>
#include <cppurses/painter/palette/gameboy_pocket.hpp>
#include <cppurses/painter/palette/macintosh_ii.hpp>
#include <cppurses/painter/palette/msx.hpp>
#include <cppurses/painter/palette/nes.hpp>
#include <cppurses/painter/palette/savanna.hpp>
#include <cppurses/painter/palette/secam.hpp>
#include <cppurses/painter/palette/stormy6.hpp>
#include <cppurses/painter/palette/teletext.hpp>
#include <cppurses/painter/palette/thomson_m05.hpp>
#include <cppurses/painter/palette/windows.hpp>
#include <cppurses/painter/palette/windows_console.hpp>
#include <cppurses/painter/palette/zx_spectrum.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/terminal/terminal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widgets/color_select.hpp>
#include <cppurses/widget/widgets/cycle_box.hpp>
#include "cppurses/painter/color.hpp"
#include "cppurses/painter/glyph_string.hpp"

namespace colors {

class Palette_picker : public cppurses::Labeled_cycle_box {
   public:
    sig::Signal<void(cppurses::Color_palette)> palette_picked;

   public:
    Palette_picker() : Labeled_cycle_box{"Palette"}
    {
        using namespace cppurses;
        this->append_option(L"Dawn Bringer 16", dawn_bringer16::palette);
        this->append_option(L"Dawn Bringer 32", dawn_bringer32::palette);
        this->append_option(L"Basic 16", basic::palette);
        this->append_option(L"Basic 8", basic8::palette);
        this->append_option(L"EN 4", en4::palette);
        this->append_option(L"Stormy 6", stormy6::palette);
        this->append_option(L"Teletext", teletext::palette);
        this->append_option(L"Gameboy", gameboy::palette);
        this->append_option(L"Gameboy Pocket", gameboy_pocket::palette);
        this->append_option(L"NES", nes::palette);
        this->append_option(L"CGA", cga::palette);
        this->append_option(L"Amstrad CPC", amstrad_cpc::palette);
        this->append_option(L"Early MSFT Windows", windows::palette);
        this->append_option(L"Apple II", apple_ii::palette);
        this->append_option(L"Macintosh II", macintosh_ii::palette);
        this->append_option(L"MSX", msx::palette);
        this->append_option(L"ZX Spectrum", zx_spectrum::palette);
        this->append_option(L"Thomson M05", thomson_m05::palette);
        this->append_option(L"SECAM", secam::palette);
        this->append_option(L"Commodore VIC-20", commodore_vic20::palette);
        this->append_option(L"Commodore 64", commodore_64::palette);
        this->append_option(L"Windows Console", windows_console::palette);
        this->append_option(L"Atelier Savanna", savanna::palette);
        this->append_option(L"Ashes", ashes::palette);
    }

   private:
    /// Adds an entry to the cycle box for the given palette with \p title.
    void append_option(cppurses::Glyph_string title,
                       cppurses::Color_palette const& pal)
    {
        this->cycle_box.add_option(std::move(title)).connect([this, pal]() {
            palette_picked(pal);
        });
    }
};

class Palette_demo : public cppurses::layout::Vertical<> {
   public:
    cppurses::Color_select& palette_view =
        this->make_child<cppurses::Color_select>(true);
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
