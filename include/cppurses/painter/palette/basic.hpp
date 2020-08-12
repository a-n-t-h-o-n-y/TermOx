#ifndef CPPURSES_PAINTER_PALETTE_BASIC_HPP
#define CPPURSES_PAINTER_PALETTE_BASIC_HPP
#include <cppurses/painter/color.hpp>

namespace cppurses::basic {

constexpr auto Black   = Color{0};
constexpr auto Maroon  = Color{1};
constexpr auto Green   = Color{2};
constexpr auto Olive   = Color{3};
constexpr auto Navy    = Color{4};
constexpr auto Purple  = Color{5};
constexpr auto Teal    = Color{6};
constexpr auto Silver  = Color{7};
constexpr auto Grey    = Color{8};
constexpr auto Red     = Color{9};
constexpr auto Lime    = Color{10};
constexpr auto Yellow  = Color{11};
constexpr auto Blue    = Color{12};
constexpr auto Fuchsia = Color{13};
constexpr auto Aqua    = Color{14};
constexpr auto White   = Color{15};

inline auto const palette = Color_palette{
    /* clang-format off */
    {Black,   ANSI{0}},
    {Maroon,  ANSI{1}},
    {Green,   ANSI{2}},
    {Olive,   ANSI{3}},
    {Navy,    ANSI{4}},
    {Purple,  ANSI{5}},
    {Teal,    ANSI{6}},
    {Silver,  ANSI{7}},
    {Grey,    ANSI{8}},
    {Red,     ANSI{9}},
    {Lime,    ANSI{10}},
    {Yellow,  ANSI{11}},
    {Blue,    ANSI{12}},
    {Fuchsia, ANSI{13}},
    {Aqua,    ANSI{14}},
    {White,   ANSI{15}},
    /* clang-format on */
};

}  // namespace cppurses::basic
#endif  // CPPURSES_PAINTER_PALETTE_BASIC_HPP
