#ifndef TERMOX_PAINTER_PALETTE_BASIC_HPP
#define TERMOX_PAINTER_PALETTE_BASIC_HPP
#include <termox/painter/color.hpp>

namespace ox::basic {

inline constexpr auto Black   = Color::Background;
inline constexpr auto Maroon  = Color{1};
inline constexpr auto Green   = Color{2};
inline constexpr auto Olive   = Color{3};
inline constexpr auto Navy    = Color{4};
inline constexpr auto Purple  = Color{5};
inline constexpr auto Teal    = Color{6};
inline constexpr auto Silver  = Color{7};
inline constexpr auto Grey    = Color{8};
inline constexpr auto Red     = Color{9};
inline constexpr auto Lime    = Color{10};
inline constexpr auto Yellow  = Color{11};
inline constexpr auto Blue    = Color{12};
inline constexpr auto Fuchsia = Color{13};
inline constexpr auto Aqua    = Color{14};
inline constexpr auto White   = Color::Foreground;

/// The Built-in Default Terminal 16 Color Palette
inline auto const palette = Palette{
    /* clang-format off */
    {Black,   Color_index{0}},
    {Maroon,  Color_index{1}},
    {Green,   Color_index{2}},
    {Olive,   Color_index{3}},
    {Navy,    Color_index{4}},
    {Purple,  Color_index{5}},
    {Teal,    Color_index{6}},
    {Silver,  Color_index{7}},
    {Grey,    Color_index{8}},
    {Red,     Color_index{9}},
    {Lime,    Color_index{10}},
    {Yellow,  Color_index{11}},
    {Blue,    Color_index{12}},
    {Fuchsia, Color_index{13}},
    {Aqua,    Color_index{14}},
    {White,   Color_index{15}},
    /* clang-format on */
};

}  // namespace ox::basic
#endif  // TERMOX_PAINTER_PALETTE_BASIC_HPP
