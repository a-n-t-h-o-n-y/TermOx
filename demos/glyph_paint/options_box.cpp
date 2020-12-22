#include "options_box.hpp"

#include <array>
#include <utility>

#include <termox/painter/color.hpp>
#include <termox/painter/palette/amstrad_cpc.hpp>
#include <termox/painter/palette/apple_ii.hpp>
#include <termox/painter/palette/ashes.hpp>
#include <termox/painter/palette/basic.hpp>
#include <termox/painter/palette/basic8.hpp>
#include <termox/painter/palette/cga.hpp>
#include <termox/painter/palette/commodore_64.hpp>
#include <termox/painter/palette/commodore_vic20.hpp>
#include <termox/painter/palette/dawn_bringer16.hpp>
#include <termox/painter/palette/dawn_bringer32.hpp>
#include <termox/painter/palette/en4.hpp>
#include <termox/painter/palette/gameboy.hpp>
#include <termox/painter/palette/gameboy_pocket.hpp>
#include <termox/painter/palette/macintosh_ii.hpp>
#include <termox/painter/palette/msx.hpp>
#include <termox/painter/palette/nes.hpp>
#include <termox/painter/palette/savanna.hpp>
#include <termox/painter/palette/secam.hpp>
#include <termox/painter/palette/stormy6.hpp>
#include <termox/painter/palette/teletext.hpp>
#include <termox/painter/palette/thomson_m05.hpp>
#include <termox/painter/palette/windows.hpp>
#include <termox/painter/palette/windows_console.hpp>
#include <termox/painter/palette/zx_spectrum.hpp>
#include <termox/system/system.hpp>
#include <termox/terminal/terminal.hpp>
#include <termox/widget/pipe.hpp>

namespace paint {

Options_B::Options_B()
{
    using namespace ox;
    using namespace ox::pipe;

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
