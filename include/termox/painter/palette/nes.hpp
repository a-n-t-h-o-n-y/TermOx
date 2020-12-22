#ifndef TERMOX_PAINTER_PALETTE_NES_HPP
#define TERMOX_PAINTER_PALETTE_NES_HPP
#include <termox/painter/color.hpp>

namespace ox::nes {

constexpr auto Black             = Color::Background;
constexpr auto Alabaster         = Color{1};
constexpr auto Silver            = Color{2};
constexpr auto Boulder           = Color{3};
constexpr auto Anakiwa           = Color{4};
constexpr auto Dodger_blue       = Color{5};
constexpr auto Azure             = Color{6};
constexpr auto Blue              = Color{7};
constexpr auto Perfume           = Color{8};
constexpr auto Cornflower_blue   = Color{9};
constexpr auto Blue_ribbon       = Color{10};
constexpr auto Dark_blue         = Color{11};
constexpr auto Pink_illusion     = Color{12};
constexpr auto Portage           = Color{13};
constexpr auto Electric_violet   = Color{14};
constexpr auto White             = Color::Foreground;
constexpr auto Purple_heart      = Color{16};
constexpr auto Classic_rose      = Color{17};
constexpr auto Blush_pink        = Color{18};
constexpr auto Purple_pizzazz    = Color{19};
constexpr auto Flirt             = Color{20};
constexpr auto Illusion          = Color{21};
constexpr auto Brilliant_rose    = Color{22};
constexpr auto Razzmatazz        = Color{23};
constexpr auto Carmine           = Color{24};
constexpr auto Pancho            = Color{25};
constexpr auto Bittersweet       = Color{26};
constexpr auto Scarlet           = Color{27};
constexpr auto Bright_red        = Color{28};
constexpr auto Cape_honey        = Color{29};
constexpr auto Yellow_orange     = Color{30};
constexpr auto Christine         = Color{31};
constexpr auto Red_berry         = Color{32};
constexpr auto Marigold_yellow   = Color{33};
constexpr auto Selective_yellow  = Color{34};
constexpr auto Pirate_gold       = Color{35};
constexpr auto Saddle_brown      = Color{36};
constexpr auto Mindaro           = Color{37};
constexpr auto Lime              = Color{38};
constexpr auto Green             = Color{39};
constexpr auto Japanese_laurel   = Color{40};
constexpr auto Gossip            = Color{41};
constexpr auto Pastel_green      = Color{42};
constexpr auto Japanese_laurel_2 = Color{43};
constexpr auto Camarone          = Color{44};
constexpr auto Ice_cold          = Color{45};
constexpr auto Screamin_green    = Color{46};
constexpr auto Green_haze        = Color{47};
constexpr auto Camarone_2        = Color{48};
constexpr auto Cyan              = Color{49};
constexpr auto Bright_turquoise  = Color{50};
constexpr auto Teal              = Color{51};
constexpr auto Astronaut_blue    = Color{52};
constexpr auto Carousel_pink     = Color{53};
constexpr auto Boulder_2         = Color{54};

/// Nintendo Entertainment System Color Palette
/** https://lospec.com/palette-list/nintendo-entertainment-system */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,             ANSI{16}, 0x000000},
    {Alabaster,         ANSI{17}, 0xf8f8f8},
    {Silver,            ANSI{18}, 0xbcbcbc},
    {Boulder,           ANSI{19}, 0x7c7c7c},
    {Anakiwa,           ANSI{20}, 0xa4e4fc},
    {Dodger_blue,       ANSI{21}, 0x3cbcfc},
    {Azure,             ANSI{22}, 0x0078f8},
    {Blue,              ANSI{23}, 0x0000fc},
    {Perfume,           ANSI{24}, 0xb8b8f8},
    {Cornflower_blue,   ANSI{25}, 0x6888fc},
    {Blue_ribbon,       ANSI{26}, 0x0058f8},
    {Dark_blue,         ANSI{27}, 0x0000bc},
    {Pink_illusion,     ANSI{28}, 0xd8b8f8},
    {Portage,           ANSI{29}, 0x9878f8},
    {Electric_violet,   ANSI{30}, 0x6844fc},
    {White,             ANSI{31}, 0xfcfcfc},
    {Purple_heart,      ANSI{32}, 0x4428bc},
    {Classic_rose,      ANSI{33}, 0xf8b8f8},
    {Blush_pink,        ANSI{34}, 0xf878f8},
    {Purple_pizzazz,    ANSI{35}, 0xd800cc},
    {Flirt,             ANSI{36}, 0x940084},
    {Illusion,          ANSI{37}, 0xf8a4c0},
    {Brilliant_rose,    ANSI{38}, 0xf85898},
    {Razzmatazz,        ANSI{39}, 0xe40058},
    {Carmine,           ANSI{40}, 0xa80020},
    {Pancho,            ANSI{41}, 0xf0d0b0},
    {Bittersweet,       ANSI{42}, 0xf87858},
    {Scarlet,           ANSI{43}, 0xf83800},
    {Bright_red,        ANSI{44}, 0xa81000},
    {Cape_honey,        ANSI{45}, 0xfce0a8},
    {Yellow_orange,     ANSI{46}, 0xfca044},
    {Christine,         ANSI{47}, 0xe45c10},
    {Red_berry,         ANSI{48}, 0x881400},
    {Marigold_yellow,   ANSI{49}, 0xf8d878},
    {Selective_yellow,  ANSI{50}, 0xf8b800},
    {Pirate_gold,       ANSI{51}, 0xac7c00},
    {Saddle_brown,      ANSI{52}, 0x503000},
    {Mindaro,           ANSI{53}, 0xd8f878},
    {Lime,              ANSI{54}, 0xb8f818},
    {Green,             ANSI{55}, 0x00b800},
    {Japanese_laurel,   ANSI{56}, 0x007800},
    {Gossip,            ANSI{57}, 0xb8f8b8},
    {Pastel_green,      ANSI{58}, 0x58d854},
    {Japanese_laurel_2, ANSI{59}, 0x00a800},
    {Camarone,          ANSI{60}, 0x006800},
    {Ice_cold,          ANSI{61}, 0xb8f8d8},
    {Screamin_green,    ANSI{62}, 0x58f898},
    {Green_haze,        ANSI{63}, 0x00a844},
    {Camarone_2,        ANSI{64}, 0x005800},
    {Cyan,              ANSI{65}, 0x00fcfc},
    {Bright_turquoise,  ANSI{66}, 0x00e8d8},
    {Teal,              ANSI{67}, 0x008888},
    {Astronaut_blue,    ANSI{68}, 0x004058},
    {Carousel_pink,     ANSI{69}, 0xf8d8f8},
    {Boulder_2,         ANSI{70}, 0x787878},
    /* clang-format on */
};

}  // namespace ox::nes
#endif  // TERMOX_PAINTER_PALETTE_NES_HPP
