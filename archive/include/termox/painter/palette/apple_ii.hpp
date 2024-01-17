#ifndef TERMOX_PAINTER_PALETTE_APPLE_II_HPP
#define TERMOX_PAINTER_PALETTE_APPLE_II_HPP
#include <termox/painter/color.hpp>

namespace ox::apple_ii {

inline constexpr auto Black        = Color::Background;
inline constexpr auto Dark_pink    = Color{1};
inline constexpr auto Green        = Color{2};
inline constexpr auto Olive        = Color{3};
inline constexpr auto Blue         = Color{4};
inline constexpr auto Violet       = Color{5};
inline constexpr auto Aqua         = Color{6};
inline constexpr auto Lavendar     = Color{7};
inline constexpr auto Gray         = Color{8};
inline constexpr auto Pink         = Color{9};
inline constexpr auto Lime         = Color{10};
inline constexpr auto Yellow_green = Color{11};
inline constexpr auto Magenta      = Color{12};
inline constexpr auto Orange       = Color{13};
inline constexpr auto White        = Color::Foreground;

/// Apple II 15 Color Palette
/** https://lospec.com/palette-list/apple-ii */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,        RGB{0x000000}},
    {Dark_pink,    RGB{0x843d52}},
    {Green,        RGB{0x006752}},
    {Olive,        RGB{0x515c16}},
    {Blue,         RGB{0x00a6f0}},
    {Violet,       RGB{0x514888}},
    {Aqua,         RGB{0x98dbc9}},
    {Lavendar,     RGB{0xc8c1f7}},
    {Gray,         RGB{0x919191}},
    {Pink,         RGB{0xf5b7c9}},
    {Lime,         RGB{0x00c82c}},
    {Yellow_green, RGB{0xc9d199}},
    {Magenta,      RGB{0xe85def}},
    {Orange,       RGB{0xea7d27}},
    {White,        RGB{0xffffff}},
    /* clang-format on */
};

}  // namespace ox::apple_ii
#endif  // TERMOX_PAINTER_PALETTE_APPLE_II_HPP
