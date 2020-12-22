#ifndef TERMOX_PAINTER_PALETTE_COMMODORE_VIC20_HPP
#define TERMOX_PAINTER_PALETTE_COMMODORE_VIC20_HPP
#include <termox/painter/color.hpp>

namespace ox::commodore_vic20 {

constexpr auto Black        = Color::Background;
constexpr auto Dark_red     = Color{1};
constexpr auto Green        = Color{2};
constexpr auto Olive        = Color{3};
constexpr auto Blue         = Color{4};
constexpr auto Violet       = Color{5};
constexpr auto Brown        = Color{6};
constexpr auto Cyan         = Color{7};
constexpr auto Red          = Color{8};
constexpr auto Light_green  = Color{9};
constexpr auto Yellow       = Color{10};
constexpr auto Light_blue   = Color{11};
constexpr auto Light_violet = Color{12};
constexpr auto Sand         = Color{13};
constexpr auto Light_cyan   = Color{14};
constexpr auto White        = Color::Foreground;

/// Commodore VIC-20 16 Color Palette
/** https://lospec.com/palette-list/commodore-vic-20 */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,        ANSI{16}, 0x000000},
    {Dark_red,     ANSI{17}, 0x772d26},
    {Green,        ANSI{18}, 0x559e4a},
    {Olive,        ANSI{19}, 0xbdcc71},
    {Blue,         ANSI{20}, 0x42348b},
    {Violet,       ANSI{21}, 0xa85fb4},
    {Brown,        ANSI{22}, 0xa8734a},
    {Cyan,         ANSI{23}, 0x85d4dc},
    {Red,          ANSI{24}, 0xb66862},
    {Light_green,  ANSI{25}, 0x92df87},
    {Yellow,       ANSI{26}, 0xffffb0},
    {Light_blue,   ANSI{27}, 0x7e70ca},
    {Light_violet, ANSI{28}, 0xe99df5},
    {Sand,         ANSI{29}, 0xe9b287},
    {Light_cyan,   ANSI{30}, 0xc5ffff},
    {White,        ANSI{31}, 0xffffff},
    /* clang-format on */
};

}  // namespace ox::commodore_vic20
#endif  // TERMOX_PAINTER_PALETTE_COMMODORE_VIC20_HPP
