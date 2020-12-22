#ifndef TERMOX_PAINTER_PALETTE_AMSTRAD_CPC_HPP
#define TERMOX_PAINTER_PALETTE_AMSTRAD_CPC_HPP
#include <termox/painter/color.hpp>

namespace ox::amstrad_cpc {

constexpr auto Black        = Color::Background;
constexpr auto Maroon       = Color{1};
constexpr auto Green        = Color{2};
constexpr auto Olive        = Color{3};
constexpr auto Navy         = Color{4};
constexpr auto Violet       = Color{5};
constexpr auto Blue         = Color{6};
constexpr auto Gray         = Color{7};
constexpr auto Malibu       = Color{8};
constexpr auto Red          = Color{9};
constexpr auto Spring_green = Color{10};
constexpr auto Yellow       = Color{11};
constexpr auto Azure        = Color{12};
constexpr auto Orange       = Color{13};
constexpr auto Teal         = Color{14};
constexpr auto White        = Color::Foreground;
constexpr auto Light_red    = Color{16};
constexpr auto Light_yellow = Color{17};
constexpr auto Lime         = Color{18};
constexpr auto Yellow_green = Color{19};
constexpr auto Mint         = Color{20};
constexpr auto Cyan         = Color{21};
constexpr auto Anakiwa      = Color{22};
constexpr auto Pink         = Color{23};
constexpr auto Magenta      = Color{24};
constexpr auto Rose         = Color{25};
constexpr auto Eggplant     = Color{26};

/// Amstrad CPC 27 Color Palette
/** https://lospec.com/palette-list/amstrad-cpc */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,        ANSI{16}, 0x040404},
    {Maroon,       ANSI{17}, 0x800000},
    {Green,        ANSI{18}, 0x008000},
    {Olive,        ANSI{19}, 0x808000},
    {Navy,         ANSI{20}, 0x00007f},
    {Violet,       ANSI{21}, 0x7f00ff},
    {Blue,         ANSI{22}, 0x0000ff},
    {Gray,         ANSI{23}, 0x808080},
    {Malibu,       ANSI{24}, 0x8080ff},
    {Red,          ANSI{25}, 0xff0000},
    {Spring_green, ANSI{26}, 0x01ff80},
    {Yellow,       ANSI{27}, 0xffff00},
    {Azure,        ANSI{28}, 0x0080ff},
    {Orange,       ANSI{29}, 0xff7f00},
    {Teal,         ANSI{30}, 0x008080},
    {White,        ANSI{31}, 0xffffff},
    {Light_red,    ANSI{32}, 0xff8080},
    {Light_yellow, ANSI{33}, 0xffff80},
    {Lime,         ANSI{34}, 0x01ff00},
    {Yellow_green, ANSI{35}, 0x80ff00},
    {Mint,         ANSI{36}, 0x80ff80},
    {Cyan,         ANSI{37}, 0x01ffff},
    {Anakiwa,      ANSI{38}, 0x80ffff},
    {Pink,         ANSI{39}, 0xff80ff},
    {Magenta,      ANSI{40}, 0xff00ff },
    {Rose,         ANSI{41}, 0xff0080},
    {Eggplant,     ANSI{42}, 0x800080},
    /* clang-format on */
};

}  // namespace ox::amstrad_cpc
#endif  // TERMOX_PAINTER_PALETTE_AMSTRAD_CPC_HPP
