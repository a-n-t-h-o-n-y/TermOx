#ifndef TERMOX_PAINTER_PALETTE_WINDOWS_CONSOLE_HPP
#define TERMOX_PAINTER_PALETTE_WINDOWS_CONSOLE_HPP
#include <termox/painter/color.hpp>

namespace ox::windows_console {

constexpr auto Black       = Color::Background;
constexpr auto Red         = Color{1};
constexpr auto Green       = Color{2};
constexpr auto Olive       = Color{3};
constexpr auto Blue        = Color{4};
constexpr auto Violet      = Color{5};
constexpr auto Azure       = Color{6};
constexpr auto Silver      = Color{7};
constexpr auto Gray        = Color{8};
constexpr auto Light_red   = Color{9};
constexpr auto Light_green = Color{10};
constexpr auto Yellow      = Color{11};
constexpr auto Light_blue  = Color{12};
constexpr auto Magenta     = Color{13};
constexpr auto Cyan        = Color{14};
constexpr auto White       = Color::Foreground;

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
