#ifndef TERMOX_PAINTER_PALETTE_APPLE_II_HPP
#define TERMOX_PAINTER_PALETTE_APPLE_II_HPP
#include <termox/painter/color.hpp>

namespace ox::apple_ii {

constexpr auto Black        = Color::Background;
constexpr auto Dark_pink    = Color{1};
constexpr auto Green        = Color{2};
constexpr auto Olive        = Color{3};
constexpr auto Blue         = Color{4};
constexpr auto Violet       = Color{5};
constexpr auto Aqua         = Color{6};
constexpr auto Lavendar     = Color{7};
constexpr auto Gray         = Color{8};
constexpr auto Pink         = Color{9};
constexpr auto Lime         = Color{10};
constexpr auto Yellow_green = Color{11};
constexpr auto Magenta      = Color{12};
constexpr auto Orange       = Color{13};
constexpr auto White        = Color::Foreground;

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
