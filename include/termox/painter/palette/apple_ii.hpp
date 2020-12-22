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
    {Black,        ANSI{16}, 0x000000},
    {Dark_pink,    ANSI{17}, 0x843d52},
    {Green,        ANSI{18}, 0x006752},
    {Olive,        ANSI{19}, 0x515c16},
    {Blue,         ANSI{20}, 0x00a6f0},
    {Violet,       ANSI{21}, 0x514888},
    {Aqua,         ANSI{22}, 0x98dbc9},
    {Lavendar,     ANSI{23}, 0xc8c1f7},
    {Gray,         ANSI{24}, 0x919191},
    {Pink,         ANSI{25}, 0xf5b7c9},
    {Lime,         ANSI{26}, 0x00c82c},
    {Yellow_green, ANSI{27}, 0xc9d199},
    {Magenta,      ANSI{28}, 0xe85def},
    {Orange,       ANSI{29}, 0xea7d27},
    {White,        ANSI{30}, 0xffffff},
    /* clang-format on */
};

}  // namespace ox::apple_ii
#endif  // TERMOX_PAINTER_PALETTE_APPLE_II_HPP
