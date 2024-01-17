#ifndef TERMOX_PAINTER_PALETTE_ZX_SPECTRUM_HPP
#define TERMOX_PAINTER_PALETTE_ZX_SPECTRUM_HPP
#include <termox/painter/color.hpp>

namespace ox::zx_spectrum {

inline constexpr auto Black       = Color::Background;
inline constexpr auto Dark_red    = Color{1};
inline constexpr auto Dark_green  = Color{2};
inline constexpr auto Olive       = Color{3};
inline constexpr auto Dark_blue   = Color{4};
inline constexpr auto Dark_violet = Color{5};
inline constexpr auto Dark_cyan   = Color{6};
inline constexpr auto Silver      = Color{7};
inline constexpr auto Black_2     = Color{8};
inline constexpr auto Red         = Color{9};
inline constexpr auto Green       = Color{10};
inline constexpr auto Yellow      = Color{11};
inline constexpr auto Blue        = Color{12};
inline constexpr auto Violet      = Color{13};
inline constexpr auto Cyan        = Color{14};
inline constexpr auto White       = Color::Foreground;

/// ZX Spectrum 16 Color Palette
/** https://lospec.com/palette-list/zx-spectrum */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,       RGB{0x000000}},
    {Dark_red,    RGB{0xd62816}},
    {Dark_green,  RGB{0x00c525}},
    {Olive,       RGB{0xccc82a}},
    {Dark_blue,   RGB{0x0022c7}},
    {Dark_violet, RGB{0xd433c7}},
    {Dark_cyan,   RGB{0x00c7c9}},
    {Silver,      RGB{0xcacaca}},
    {Black_2,     RGB{0x000000}},
    {Red,         RGB{0xff331c}},
    {Green,       RGB{0x00f92f}},
    {Yellow,      RGB{0xfffc36}},
    {Blue,        RGB{0x002bfb}},
    {Violet,      RGB{0xff40fc}},
    {Cyan,        RGB{0x00fbfe}},
    {White,       RGB{0xffffff}},
    /* clang-format on */
};

}  // namespace ox::zx_spectrum
#endif  // TERMOX_PAINTER_PALETTE_ZX_SPECTRUM_HPP
