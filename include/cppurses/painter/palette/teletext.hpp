#ifndef CPPURSES_PAINTER_PALETTE_TELETEXT_HPP
#define CPPURSES_PAINTER_PALETTE_TELETEXT_HPP
#include <cppurses/painter/color.hpp>

namespace cppurses::teletext {

constexpr auto Black   = Color::Background;
constexpr auto Blue    = Color{4};
constexpr auto Red     = Color{1};
constexpr auto Magenta = Color{5};
constexpr auto Lime    = Color{2};
constexpr auto Cyan    = Color{6};
constexpr auto Yellow  = Color{3};
constexpr auto White   = Color::Foreground;

/// World System Teletext Level 1 Color Palette
inline auto const palette = Color_palette{
    /* clang-format off */
    {Black,   ANSI{16}, 0x000000},
    {Blue,    ANSI{17}, 0x0000ff},
    {Red,     ANSI{18}, 0xff0000},
    {Magenta, ANSI{19}, 0xff00ff},
    {Lime,    ANSI{20}, 0x00ff00},
    {Cyan,    ANSI{21}, 0x00ffff},
    {Yellow,  ANSI{22}, 0xffff00},
    {White,   ANSI{23}, 0xffffff},
    /* clang-format on */
};

}  // namespace cppurses::teletext
#endif  // CPPURSES_PAINTER_PALETTE_TELETEXT_HPP
