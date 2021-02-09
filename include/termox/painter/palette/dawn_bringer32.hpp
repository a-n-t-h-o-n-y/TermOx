#ifndef TERMOX_PAINTER_PALETTE_DAWN_BRINGER32_HPP
#define TERMOX_PAINTER_PALETTE_DAWN_BRINGER32_HPP
#include <termox/painter/color.hpp>

namespace ox::dawn_bringer32 {

constexpr auto Black          = Color::Background;
constexpr auto Well_read      = Color{1};
constexpr auto Ocean_green    = Color{2};
constexpr auto Golden_fizz    = Color{3};
constexpr auto Royal_blue     = Color{4};
constexpr auto Hopbush        = Color{5};
constexpr auto Atlantis       = Color{6};
constexpr auto Gull_gray      = Color{7};
constexpr auto Chicago        = Color{8};
constexpr auto Roman          = Color{9};
constexpr auto Di_serria      = Color{10};
constexpr auto Buccaneer      = Color{11};
constexpr auto Steel_gray     = Color{12};
constexpr auto Hot_cinnamon   = Color{13};
constexpr auto Corduroy       = Color{14};
constexpr auto White          = Color::Foreground;
constexpr auto Calypso        = Color{16};
constexpr auto Potters_clay   = Color{17};
constexpr auto Malibu         = Color{18};
constexpr auto Turquoise_blue = Color{19};
constexpr auto Hawkes_blue    = Color{20};
constexpr auto Chalky         = Color{21};
constexpr auto East_bay       = Color{22};
constexpr auto Mobster        = Color{23};
constexpr auto Cape_cod       = Color{24};
constexpr auto Livid_brown    = Color{25};
constexpr auto Affair         = Color{26};
constexpr auto Chalet_green   = Color{27};
constexpr auto Conifer        = Color{28};
constexpr auto Woodland       = Color{29};
constexpr auto Sycamore       = Color{30};
constexpr auto Pesto          = Color{31};

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
