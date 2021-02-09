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
    {Black,             RGB{0x000000}},
    {Alabaster,         RGB{0xf8f8f8}},
    {Silver,            RGB{0xbcbcbc}},
    {Boulder,           RGB{0x7c7c7c}},
    {Anakiwa,           RGB{0xa4e4fc}},
    {Dodger_blue,       RGB{0x3cbcfc}},
    {Azure,             RGB{0x0078f8}},
    {Blue,              RGB{0x0000fc}},
    {Perfume,           RGB{0xb8b8f8}},
    {Cornflower_blue,   RGB{0x6888fc}},
    {Blue_ribbon,       RGB{0x0058f8}},
    {Dark_blue,         RGB{0x0000bc}},
    {Pink_illusion,     RGB{0xd8b8f8}},
    {Portage,           RGB{0x9878f8}},
    {Electric_violet,   RGB{0x6844fc}},
    {White,             RGB{0xfcfcfc}},
    {Purple_heart,      RGB{0x4428bc}},
    {Classic_rose,      RGB{0xf8b8f8}},
    {Blush_pink,        RGB{0xf878f8}},
    {Purple_pizzazz,    RGB{0xd800cc}},
    {Flirt,             RGB{0x940084}},
    {Illusion,          RGB{0xf8a4c0}},
    {Brilliant_rose,    RGB{0xf85898}},
    {Razzmatazz,        RGB{0xe40058}},
    {Carmine,           RGB{0xa80020}},
    {Pancho,            RGB{0xf0d0b0}},
    {Bittersweet,       RGB{0xf87858}},
    {Scarlet,           RGB{0xf83800}},
    {Bright_red,        RGB{0xa81000}},
    {Cape_honey,        RGB{0xfce0a8}},
    {Yellow_orange,     RGB{0xfca044}},
    {Christine,         RGB{0xe45c10}},
    {Red_berry,         RGB{0x881400}},
    {Marigold_yellow,   RGB{0xf8d878}},
    {Selective_yellow,  RGB{0xf8b800}},
    {Pirate_gold,       RGB{0xac7c00}},
    {Saddle_brown,      RGB{0x503000}},
    {Mindaro,           RGB{0xd8f878}},
    {Lime,              RGB{0xb8f818}},
    {Green,             RGB{0x00b800}},
    {Japanese_laurel,   RGB{0x007800}},
    {Gossip,            RGB{0xb8f8b8}},
    {Pastel_green,      RGB{0x58d854}},
    {Japanese_laurel_2, RGB{0x00a800}},
    {Camarone,          RGB{0x006800}},
    {Ice_cold,          RGB{0xb8f8d8}},
    {Screamin_green,    RGB{0x58f898}},
    {Green_haze,        RGB{0x00a844}},
    {Camarone_2,        RGB{0x005800}},
    {Cyan,              RGB{0x00fcfc}},
    {Bright_turquoise,  RGB{0x00e8d8}},
    {Teal,              RGB{0x008888}},
    {Astronaut_blue,    RGB{0x004058}},
    {Carousel_pink,     RGB{0xf8d8f8}},
    {Boulder_2,         RGB{0x787878}},
    /* clang-format on */
};

}  // namespace ox::nes
#endif  // TERMOX_PAINTER_PALETTE_NES_HPP
