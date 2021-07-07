#ifndef TERMOX_PAINTER_PALETTE_COMMODORE_VIC20_HPP
#define TERMOX_PAINTER_PALETTE_COMMODORE_VIC20_HPP
#include <termox/painter/color.hpp>

namespace ox::commodore_vic20 {

inline constexpr auto Black        = Color::Background;
inline constexpr auto Dark_red     = Color{1};
inline constexpr auto Green        = Color{2};
inline constexpr auto Olive        = Color{3};
inline constexpr auto Blue         = Color{4};
inline constexpr auto Violet       = Color{5};
inline constexpr auto Brown        = Color{6};
inline constexpr auto Cyan         = Color{7};
inline constexpr auto Red          = Color{8};
inline constexpr auto Light_green  = Color{9};
inline constexpr auto Yellow       = Color{10};
inline constexpr auto Light_blue   = Color{11};
inline constexpr auto Light_violet = Color{12};
inline constexpr auto Sand         = Color{13};
inline constexpr auto Light_cyan   = Color{14};
inline constexpr auto White        = Color::Foreground;

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
