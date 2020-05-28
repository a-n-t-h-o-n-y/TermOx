#ifndef CPPURSES_PAINTER_PALETTE_DAWN_BRINGER32_HPP
#define CPPURSES_PAINTER_PALETTE_DAWN_BRINGER32_HPP
#include <cppurses/painter/color.hpp>

namespace cppurses::dawn_bringer32 {

constexpr auto Black          = Color{0};
constexpr auto Steel_gray     = Color{12};
constexpr auto Livid_brown    = Color{25};
constexpr auto Buccaneer      = Color{11};
constexpr auto Potters_clay   = Color{17};
constexpr auto Hot_cinnamon   = Color{13};
constexpr auto Di_serria      = Color{10};
constexpr auto White          = Color{7};
constexpr auto Chalky         = Color{21};
constexpr auto Golden_fizz    = Color{3};
constexpr auto Conifer        = Color{28};
constexpr auto Atlantis       = Color{6};
constexpr auto Ocean_green    = Color{2};
constexpr auto Chalet_green   = Color{27};
constexpr auto Woodland       = Color{29};
constexpr auto Cape_cod       = Color{24};
constexpr auto East_bay       = Color{22};
constexpr auto Calypso        = Color{16};
constexpr auto Royal_blue     = Color{4};
constexpr auto Malibu         = Color{18};
constexpr auto Turquoise_blue = Color{19};
constexpr auto Hawkes_blue    = Color{20};
constexpr auto Gull_gray      = Color{15};
constexpr auto Mobster        = Color{23};
constexpr auto Corduroy       = Color{14};
constexpr auto Chicago        = Color{8};
constexpr auto Affair         = Color{26};
constexpr auto Well_read      = Color{1};
constexpr auto Roman          = Color{9};
constexpr auto Hopbush        = Color{5};
constexpr auto Sycamore       = Color{30};
constexpr auto Pesto          = Color{31};

/* clang-format off */
inline auto const palette = True_color_palette{{
    {{Black,          ANSI{16}}, True_color{0x000000}},
    {{Steel_gray,     ANSI{17}}, True_color{0x222034}},
    {{Livid_brown,    ANSI{18}}, True_color{0x45283c}},
    {{Buccaneer,      ANSI{19}}, True_color{0x663931}},
    {{Potters_clay,   ANSI{20}}, True_color{0x8f563b}},
    {{Hot_cinnamon,   ANSI{21}}, True_color{0xdf7126}},
    {{Di_serria,      ANSI{22}}, True_color{0xd9a066}},
    {{Chalky,         ANSI{23}}, True_color{0xeec39a}},
    {{Golden_fizz,    ANSI{24}}, True_color{0xfbf236}},
    {{Conifer,        ANSI{25}}, True_color{0x99e550}},
    {{Atlantis,       ANSI{26}}, True_color{0x6abe30}},
    {{Ocean_green,    ANSI{27}}, True_color{0x37946e}},
    {{Chalet_green,   ANSI{28}}, True_color{0x4b692f}},
    {{Woodland,       ANSI{29}}, True_color{0x524b24}},
    {{Cape_cod,       ANSI{30}}, True_color{0x323c39}},
    {{East_bay,       ANSI{31}}, True_color{0x3f3f74}},
    {{Calypso,        ANSI{32}}, True_color{0x306082}},
    {{Royal_blue,     ANSI{33}}, True_color{0x5b6ee1}},
    {{Malibu,         ANSI{34}}, True_color{0x639bff}},
    {{Turquoise_blue, ANSI{35}}, True_color{0x5fcde4}},
    {{Hawkes_blue,    ANSI{36}}, True_color{0xcbdbfc}},
    {{White,          ANSI{37}}, True_color{0xffffff}},
    {{Gull_gray,      ANSI{38}}, True_color{0x9badb7}},
    {{Mobster,        ANSI{39}}, True_color{0x847e87}},
    {{Corduroy,       ANSI{40}}, True_color{0x696a6a}},
    {{Chicago,        ANSI{41}}, True_color{0x595652}},
    {{Affair,         ANSI{42}}, True_color{0x76428a}},
    {{Well_read,      ANSI{43}}, True_color{0xac3232}},
    {{Roman,          ANSI{44}}, True_color{0xd95763}},
    {{Hopbush,        ANSI{45}}, True_color{0xd77bba}},
    {{Sycamore,       ANSI{46}}, True_color{0x8f974a}},
    {{Pesto,          ANSI{47}}, True_color{0x8a6f30}},
}};
}  // namespace cppurses::dawn_bringer
#endif  // CPPURSES_PAINTER_PALETTE_DAWN_BRINGER32_HPP
