#include <termox/terminal/output.hpp>

#define SLOW_PAINT 7

#ifdef SLOW_PAINT
#    include <chrono>
#    include <thread>
#endif

#include <cstddef>
#include <type_traits>  //temp

#ifndef _XOPEN_SOURCE_EXTENDED
#    define _XOPEN_SOURCE_EXTENDED
#endif
#include <ncursesw/ncurses.h>
#undef border

#include <termox/painter/brush.hpp>
#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/painter/trait.hpp>
#include <termox/system/system.hpp>

namespace {
using namespace ox;

auto color_index(Color fg, Color bg) -> short
{
    return System::terminal.color_index(fg, bg);
}

auto trait_to_attr_t(Trait t) -> attr_t
{
    auto result = A_NORMAL;
    switch (t) {
        case Trait::Bold: result = A_BOLD; break;
        case Trait::Underline: result = A_UNDERLINE; break;
        case Trait::Standout: result = A_STANDOUT; break;
        case Trait::Dim: result = A_DIM; break;
        case Trait::Inverse: result = A_REVERSE; break;
        case Trait::Invisible: result = A_INVIS; break;
        case Trait::Blink: result = A_BLINK; break;
        case Trait::Italic: result = A_ITALIC; break;
        case Trait::None:
        case Trait::Crossed_out:
        case Trait::Double_underline: break;
    }
    return result;
}

// TODO don't need this with esc, uses Trait_count though..
auto find_attr_t(Brush brush) -> attr_t
{
    auto result = A_NORMAL;
    auto count  = 0;
    for (auto i = static_cast<std::underlying_type_t<ox::Trait>>(
             ox::Trait::Standout);
         count < 8; (++count, i <<= 1)) {
        auto const a = static_cast<Trait>(i);
        if (brush.traits.contains(a))
            result |= trait_to_attr_t(a);
    }
    return result;
}

#ifdef SLOW_PAINT
void paint_indicator(char32_t symbol)
{
    auto const color_pair      = color_index(Color::White, Color::Black);
    auto const traits          = A_NORMAL;
    char32_t const temp_sym[2] = {symbol, U'\0'};
    auto temp_display          = cchar_t{' '};
    ::setcchar(&temp_display, (wchar_t const*)temp_sym, traits, color_pair,
               nullptr);
    ::wadd_wchnstr(::stdscr, &temp_display, 1);
    refresh();
    std::this_thread::sleep_for(std::chrono::milliseconds(SLOW_PAINT));
}
#endif

/// Add \p glyph's symbol, with traits, to the screen at cursor position.
void put_wchar(Glyph glyph)
{
    auto const brush         = glyph.brush;
    auto const color_pair    = color_index(brush.foreground, brush.background);
    auto const traits        = find_attr_t(glyph.brush);
    char32_t const symbol[2] = {glyph.symbol, U'\0'};
    auto symbol_and_traits   = cchar_t{};

    ::setcchar(&symbol_and_traits, (wchar_t const*)symbol, traits, color_pair,
               nullptr);
    ::wadd_wchnstr(::stdscr, &symbol_and_traits, 1);
}
}  // namespace

namespace ox::output {

void move_cursor(std::size_t x, std::size_t y)
{
    ::wmove(::stdscr, static_cast<int>(y), static_cast<int>(x));
}

void refresh() { ::wrefresh(::stdscr); }

void put(Glyph g)
{
#ifdef SLOW_PAINT
    paint_indicator(U'X');
#endif
    put_wchar(g);
#ifdef SLOW_PAINT
    refresh();
    std::this_thread::sleep_for(std::chrono::milliseconds(SLOW_PAINT));
#endif
}

}  // namespace ox::output
