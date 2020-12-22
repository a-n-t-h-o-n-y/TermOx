#ifndef TERMOX_PAINTER_PALETTE_ZX_SPECTRUM_HPP
#define TERMOX_PAINTER_PALETTE_ZX_SPECTRUM_HPP
#include <termox/painter/color.hpp>

namespace ox::zx_spectrum {

constexpr auto Black       = Color::Background;
constexpr auto Dark_red    = Color{1};
constexpr auto Dark_green  = Color{2};
constexpr auto Olive       = Color{3};
constexpr auto Dark_blue   = Color{4};
constexpr auto Dark_violet = Color{5};
constexpr auto Dark_cyan   = Color{6};
constexpr auto Silver      = Color{7};
constexpr auto Black_2     = Color{8};
constexpr auto Red         = Color{9};
constexpr auto Green       = Color{10};
constexpr auto Yellow      = Color{11};
constexpr auto Blue        = Color{12};
constexpr auto Violet      = Color{13};
constexpr auto Cyan        = Color{14};
constexpr auto White       = Color::Foreground;

/// ZX Spectrum 16 Color Palette
/** https://lospec.com/palette-list/zx-spectrum */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,       ANSI{16}, 0x000000},
    {Dark_red,    ANSI{17}, 0xd62816},
    {Dark_green,  ANSI{18}, 0x00c525},
    {Olive,       ANSI{19}, 0xccc82a},
    {Dark_blue,   ANSI{20}, 0x0022c7},
    {Dark_violet, ANSI{21}, 0xd433c7},
    {Dark_cyan,   ANSI{22}, 0x00c7c9},
    {Silver,      ANSI{23}, 0xcacaca},
    {Black_2,     ANSI{31}, 0x000000},
    {Red,         ANSI{24}, 0xff331c},
    {Green,       ANSI{25}, 0x00f92f},
    {Yellow,      ANSI{26}, 0xfffc36},
    {Blue,        ANSI{27}, 0x002bfb},
    {Violet,      ANSI{28}, 0xff40fc},
    {Cyan,        ANSI{29}, 0x00fbfe},
    {White,       ANSI{30}, 0xffffff},
    /* clang-format on */
};

}  // namespace ox::zx_spectrum
#endif  // TERMOX_PAINTER_PALETTE_ZX_SPECTRUM_HPP
