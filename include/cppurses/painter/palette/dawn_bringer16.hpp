#ifndef CPPURSES_PAINTER_PALETTE_DAWN_BRINGER16_HPP
#define CPPURSES_PAINTER_PALETTE_DAWN_BRINGER16_HPP
#include <cppurses/painter/color.hpp>

namespace cppurses::dawn_bringer16 {

constexpr auto Black       = Color{0};
constexpr auto Red         = Color{1};
constexpr auto Green       = Color{2};
constexpr auto Yellow      = Color{3};
constexpr auto Blue        = Color{4};
constexpr auto Violet      = Color{5};
constexpr auto Light_blue  = Color{6};
constexpr auto White       = Color{7};
constexpr auto Dark_gray   = Color{8};
constexpr auto Dark_red    = Color{9};
constexpr auto Light_green = Color{10};
constexpr auto Brown       = Color{11};
constexpr auto Dark_blue   = Color{12};
constexpr auto Orange      = Color{13};
constexpr auto Gray        = Color{14};
constexpr auto Light_gray  = Color{15};

inline auto const palette = True_color_palette{{
    /* clang-format off */
    {{Black,       ANSI{16}}, True_color{0x140c1c}},
    {{Red,         ANSI{17}}, True_color{0xd04648}},
    {{Green,       ANSI{18}}, True_color{0x346524}},
    {{Yellow,      ANSI{19}}, True_color{0xdad45e}},
    {{Blue,        ANSI{20}}, True_color{0x597dce}},
    {{Violet,      ANSI{21}}, True_color{0xd2aa99}},
    {{Light_blue,  ANSI{22}}, True_color{0x6dc2ca}},
    {{White,       ANSI{23}}, True_color{0xdeeed6}},
    {{Dark_gray,   ANSI{24}}, True_color{0x4e4a4e}},
    {{Dark_red,    ANSI{25}}, True_color{0x442434}},
    {{Light_green, ANSI{26}}, True_color{0x6daa2c}},
    {{Brown,       ANSI{27}}, True_color{0x854c30}},
    {{Dark_blue,   ANSI{28}}, True_color{0x30346d}},
    {{Orange,      ANSI{29}}, True_color{0xd27d2c}},
    {{Gray,        ANSI{30}}, True_color{0x757161}},
    {{Light_gray,  ANSI{31}}, True_color{0x8595a1}},
    /* clang-format on */
}};
}  // namespace cppurses::dawn_bringer16
#endif  // CPPURSES_PAINTER_PALETTE_DAWN_BRINGER16_HPP
