#ifndef TERMOX_PAINTER_PALETTE_DAWN_BRINGER32_HPP
#define TERMOX_PAINTER_PALETTE_DAWN_BRINGER32_HPP
#include <termox/painter/color.hpp>

namespace ox::dawn_bringer32 {

inline constexpr auto Black          = Color::Background;
inline constexpr auto Well_read      = Color{1};
inline constexpr auto Ocean_green    = Color{2};
inline constexpr auto Golden_fizz    = Color{3};
inline constexpr auto Royal_blue     = Color{4};
inline constexpr auto Hopbush        = Color{5};
inline constexpr auto Atlantis       = Color{6};
inline constexpr auto Gull_gray      = Color{7};
inline constexpr auto Chicago        = Color{8};
inline constexpr auto Roman          = Color{9};
inline constexpr auto Di_serria      = Color{10};
inline constexpr auto Buccaneer      = Color{11};
inline constexpr auto Steel_gray     = Color{12};
inline constexpr auto Hot_cinnamon   = Color{13};
inline constexpr auto Corduroy       = Color{14};
inline constexpr auto White          = Color::Foreground;
inline constexpr auto Calypso        = Color{16};
inline constexpr auto Potters_clay   = Color{17};
inline constexpr auto Malibu         = Color{18};
inline constexpr auto Turquoise_blue = Color{19};
inline constexpr auto Hawkes_blue    = Color{20};
inline constexpr auto Chalky         = Color{21};
inline constexpr auto East_bay       = Color{22};
inline constexpr auto Mobster        = Color{23};
inline constexpr auto Cape_cod       = Color{24};
inline constexpr auto Livid_brown    = Color{25};
inline constexpr auto Affair         = Color{26};
inline constexpr auto Chalet_green   = Color{27};
inline constexpr auto Conifer        = Color{28};
inline constexpr auto Woodland       = Color{29};
inline constexpr auto Sycamore       = Color{30};
inline constexpr auto Pesto          = Color{31};

/// Dawn Bringer 16 Color Palette
/** https://lospec.com/palette-list/dawnbringer-32 */
inline auto const palette = Palette{
    /* clang-format off */
    {Black,          RGB{0x000001}},
    {Steel_gray,     RGB{0x222034}},
    {Livid_brown,    RGB{0x45283c}},
    {Buccaneer,      RGB{0x663931}},
    {Potters_clay,   RGB{0x8f563b}},
    {Hot_cinnamon,   RGB{0xdf7126}},
    {Di_serria,      RGB{0xd9a066}},
    {Chalky,         RGB{0xeec39a}},
    {Golden_fizz,    RGB{0xfbf236}},
    {Conifer,        RGB{0x99e550}},
    {Atlantis,       RGB{0x6abe30}},
    {Ocean_green,    RGB{0x37946e}},
    {Chalet_green,   RGB{0x4b692f}},
    {Woodland,       RGB{0x524b24}},
    {Cape_cod,       RGB{0x323c39}},
    {East_bay,       RGB{0x3f3f74}},
    {Calypso,        RGB{0x306082}},
    {Royal_blue,     RGB{0x5b6ee1}},
    {Malibu,         RGB{0x639bff}},
    {Turquoise_blue, RGB{0x5fcde4}},
    {Hawkes_blue,    RGB{0xcbdbfc}},
    {White,          RGB{0xffffff}},
    {Gull_gray,      RGB{0x9badb7}},
    {Mobster,        RGB{0x847e87}},
    {Corduroy,       RGB{0x696a6a}},
    {Chicago,        RGB{0x595652}},
    {Affair,         RGB{0x76428a}},
    {Well_read,      RGB{0xac3232}},
    {Roman,          RGB{0xd95763}},
    {Hopbush,        RGB{0xd77bba}},
    {Sycamore,       RGB{0x8f974a}},
    {Pesto,          RGB{0x8a6f30}},
    /* clang-format on */
};

}  // namespace ox::dawn_bringer32
#endif  // TERMOX_PAINTER_PALETTE_DAWN_BRINGER32_HPP
