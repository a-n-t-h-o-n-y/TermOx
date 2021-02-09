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
    {Black,        RGB{0x000000}},
    {Dark_red,     RGB{0x772d26}},
    {Green,        RGB{0x559e4a}},
    {Olive,        RGB{0xbdcc71}},
    {Blue,         RGB{0x42348b}},
    {Violet,       RGB{0xa85fb4}},
    {Brown,        RGB{0xa8734a}},
    {Cyan,         RGB{0x85d4dc}},
    {Red,          RGB{0xb66862}},
    {Light_green,  RGB{0x92df87}},
    {Yellow,       RGB{0xffffb0}},
    {Light_blue,   RGB{0x7e70ca}},
    {Light_violet, RGB{0xe99df5}},
    {Sand,         RGB{0xe9b287}},
    {Light_cyan,   RGB{0xc5ffff}},
    {White,        RGB{0xffffff}},
    /* clang-format on */
};

}  // namespace ox::commodore_vic20
#endif  // TERMOX_PAINTER_PALETTE_COMMODORE_VIC20_HPP
