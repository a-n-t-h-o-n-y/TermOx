#include "options_box.hpp"

#include <array>
#include <utility>

#include <cppurses/painter/color.hpp>
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
#include <cppurses/widget/pipe.hpp>

namespace paint {

Options_B::Options_B()
{
    using namespace cppurses;
    using namespace cppurses::pipe;

    space1 | wallpaper(L'─');
    palette_label | align_center();
    back_btn | bg(Color::Light_gray) | fg(Color::Black);
    palette_box | bg(Color::White) | fg(Color::Black);

    auto const palettes = std::array{
        std::pair{L"Dawn Bringer 16", dawn_bringer16::palette},
        std::pair{L"Dawn Bringer 32", dawn_bringer32::palette},
        std::pair{L"Basic 16", basic::palette},
        std::pair{L"Basic 8", basic8::palette},
        std::pair{L"EN 4", en4::palette},
        std::pair{L"Stormy 6", stormy6::palette},
        std::pair{L"Savanna VIM", savanna::palette},
        std::pair{L"Ashes VIM", ashes::palette},
        std::pair{L"Amstrad CPC", amstrad_cpc::palette},
        std::pair{L"Apple II", apple_ii::palette},
        std::pair{L"Macintosh II", macintosh_ii::palette},
        std::pair{L"CGA", cga::palette},
        std::pair{L"MSX", msx::palette},
        std::pair{L"SECAM", secam::palette},
        std::pair{L"Commodore 64", commodore_64::palette},
        std::pair{L"Commodore VIC-20", commodore_vic20::palette},
        std::pair{L"Gameboy", gameboy::palette},
        std::pair{L"Gameboy Pocket", gameboy_pocket::palette},
        std::pair{L"NES", nes::palette},
        std::pair{L"Teletext", teletext::palette},
        std::pair{L"Thomson M05", thomson_m05::palette},
        std::pair{L"Early Windows", windows::palette},
        std::pair{L"Windows Console", windows_console::palette},
        std::pair{L"ZX Spectrum", zx_spectrum::palette},
    };

    for (auto const& [name, palette] : palettes) {
        palette_box.add_option(name).connect(
            [p = palette] { System::terminal.set_palette(p); });
    }
}

}  // namespace paint
