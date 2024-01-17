#ifndef TERMOX_DEMOS_PALETTE_VIEW_HPP
#define TERMOX_DEMOS_PALETTE_VIEW_HPP
#include <utility>

#include <termox/termox.hpp>

namespace colors {

class Palette_picker : public ox::Labeled_cycle_box {
   public:
    sl::Signal<void(ox::Palette)> palette_picked;

   public:
    Palette_picker(Parameters = {}) : Labeled_cycle_box{"Palette"}
    {
        using namespace ox;
        *this | pipe::direct_focus();

        this->append_option(U"Dawn Bringer 16", dawn_bringer16::palette);
        this->append_option(U"Dawn Bringer 32", dawn_bringer32::palette);
        this->append_option(U"Basic 16", basic::palette);
        this->append_option(U"Basic 8", basic8::palette);
        this->append_option(U"EN 4", en4::palette);
        this->append_option(U"Stormy 6", stormy6::palette);
        this->append_option(U"Teletext", teletext::palette);
        this->append_option(U"Gameboy", gameboy::palette);
        this->append_option(U"Gameboy Pocket", gameboy_pocket::palette);
        this->append_option(U"NES", nes::palette);
        this->append_option(U"CGA", cga::palette);
        this->append_option(U"Amstrad CPC", amstrad_cpc::palette);
        this->append_option(U"Early MSFT Windows", windows::palette);
        this->append_option(U"Apple II", apple_ii::palette);
        this->append_option(U"Macintosh II", macintosh_ii::palette);
        this->append_option(U"MSX", msx::palette);
        this->append_option(U"ZX Spectrum", zx_spectrum::palette);
        this->append_option(U"Thomson M05", thomson_m05::palette);
        this->append_option(U"SECAM", secam::palette);
        this->append_option(U"Commodore VIC-20", commodore_vic20::palette);
        this->append_option(U"Commodore 64", commodore_64::palette);
        this->append_option(U"Windows Console", windows_console::palette);
        this->append_option(U"Atelier Savanna", savanna::palette);
        this->append_option(U"Ashes", ashes::palette);
    }

   private:
    /// Adds an entry to the cycle box for the given palette with \p title.
    void append_option(ox::Glyph_string title, ox::Palette const& pal)
    {
        this->cycle_box.add_option(std::move(title)).connect([this, pal]() {
            palette_picked(pal);
        });
    }
};

class Palette_view : public ox::VPair<ox::Color_select, Palette_picker> {
   public:
    ox::Color_select& palette_view = this->first;
    Palette_picker& palette_picker = this->second;

   public:
    Palette_view()
        : ox::VPair<ox::Color_select, Palette_picker>{
              {ox::Color_tile::Display::Number},
              {}}
    {
        using namespace ox::pipe;
        *this | strong_focus() | forward_focus(palette_picker) |
            on_focus_in(
                [] { ox::Terminal::set_palette(ox::dawn_bringer16::palette); });
        palette_picker.palette_picked.connect(
            [](auto const& pal) { ox::Terminal::set_palette(pal); });
    }
};

}  // namespace colors
#endif  // TERMOX_DEMOS_PALETTE_VIEW_HPP
