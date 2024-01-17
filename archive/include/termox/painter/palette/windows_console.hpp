#ifndef TERMOX_PAINTER_PALETTE_WINDOWS_CONSOLE_HPP
#define TERMOX_PAINTER_PALETTE_WINDOWS_CONSOLE_HPP
#include <termox/painter/color.hpp>

namespace ox::windows_console {

inline constexpr auto Black       = Color::Background;
inline constexpr auto Red         = Color{1};
inline constexpr auto Green       = Color{2};
inline constexpr auto Olive       = Color{3};
inline constexpr auto Blue        = Color{4};
inline constexpr auto Violet      = Color{5};
inline constexpr auto Azure       = Color{6};
inline constexpr auto Silver      = Color{7};
inline constexpr auto Gray        = Color{8};
inline constexpr auto Light_red   = Color{9};
inline constexpr auto Light_green = Color{10};
inline constexpr auto Yellow      = Color{11};
inline constexpr auto Light_blue  = Color{12};
inline constexpr auto Magenta     = Color{13};
inline constexpr auto Cyan        = Color{14};
inline constexpr auto White       = Color::Foreground;

/// New Windows Console 16 Color Palette
/** https://lospec.com/palette-list/campbell-new-windows-console */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,       RGB{0x0c0c0c}},
    {Red,         RGB{0xc50f1f}},
    {Green,       RGB{0x13a10e}},
    {Olive,       RGB{0xc19c00}},
    {Blue,        RGB{0x0037da}},
    {Violet,      RGB{0x881798}},
    {Azure,       RGB{0x3a96dd}},
    {Silver,      RGB{0xcccccc}},
    {Gray,        RGB{0x767676}},
    {Light_red,   RGB{0xe74856}},
    {Light_green, RGB{0x16c60c}},
    {Yellow,      RGB{0xf9f1a5}},
    {Light_blue,  RGB{0x3b78ff}},
    {Magenta,     RGB{0xb4009e}},
    {Cyan,        RGB{0x61d6d6}},
    {White,       RGB{0xf2f2f2}},
    /* clang-format on */
};

}  // namespace ox::windows_console
#endif  // TERMOX_PAINTER_PALETTE_WINDOWS_CONSOLE_HPP
