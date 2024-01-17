#ifndef TERMOX_PAINTER_PALETTE_AMSTRAD_CPC_HPP
#define TERMOX_PAINTER_PALETTE_AMSTRAD_CPC_HPP
#include <termox/painter/color.hpp>

namespace ox::amstrad_cpc {

inline constexpr auto Black        = Color::Background;
inline constexpr auto Maroon       = Color{1};
inline constexpr auto Green        = Color{2};
inline constexpr auto Olive        = Color{3};
inline constexpr auto Navy         = Color{4};
inline constexpr auto Violet       = Color{5};
inline constexpr auto Blue         = Color{6};
inline constexpr auto Gray         = Color{7};
inline constexpr auto Malibu       = Color{8};
inline constexpr auto Red          = Color{9};
inline constexpr auto Spring_green = Color{10};
inline constexpr auto Yellow       = Color{11};
inline constexpr auto Azure        = Color{12};
inline constexpr auto Orange       = Color{13};
inline constexpr auto Teal         = Color{14};
inline constexpr auto White        = Color::Foreground;
inline constexpr auto Light_red    = Color{16};
inline constexpr auto Light_yellow = Color{17};
inline constexpr auto Lime         = Color{18};
inline constexpr auto Yellow_green = Color{19};
inline constexpr auto Mint         = Color{20};
inline constexpr auto Cyan         = Color{21};
inline constexpr auto Anakiwa      = Color{22};
inline constexpr auto Pink         = Color{23};
inline constexpr auto Magenta      = Color{24};
inline constexpr auto Rose         = Color{25};
inline constexpr auto Eggplant     = Color{26};

/// Amstrad CPC 27 Color Palette
/** https://lospec.com/palette-list/amstrad-cpc */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,        RGB{0x040404}},
    {Maroon,       RGB{0x800000}},
    {Green,        RGB{0x008000}},
    {Olive,        RGB{0x808000}},
    {Navy,         RGB{0x00007f}},
    {Violet,       RGB{0x7f00ff}},
    {Blue,         RGB{0x0000ff}},
    {Gray,         RGB{0x808080}},
    {Malibu,       RGB{0x8080ff}},
    {Red,          RGB{0xff0000}},
    {Spring_green, RGB{0x01ff80}},
    {Yellow,       RGB{0xffff00}},
    {Azure,        RGB{0x0080ff}},
    {Orange,       RGB{0xff7f00}},
    {Teal,         RGB{0x008080}},
    {White,        RGB{0xffffff}},
    {Light_red,    RGB{0xff8080}},
    {Light_yellow, RGB{0xffff80}},
    {Lime,         RGB{0x01ff00}},
    {Yellow_green, RGB{0x80ff00}},
    {Mint,         RGB{0x80ff80}},
    {Cyan,         RGB{0x01ffff}},
    {Anakiwa,      RGB{0x80ffff}},
    {Pink,         RGB{0xff80ff}},
    {Magenta,      RGB{0xff00ff}},
    {Rose,         RGB{0xff0080}},
    {Eggplant,     RGB{0x800080}},
    /* clang-format on */
};

}  // namespace ox::amstrad_cpc
#endif  // TERMOX_PAINTER_PALETTE_AMSTRAD_CPC_HPP
