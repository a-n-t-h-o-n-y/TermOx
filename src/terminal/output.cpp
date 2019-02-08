#include <cppurses/terminal/output.hpp>

// #define SLOW_PAINT 7

#ifdef SLOW_PAINT
#include <chrono>
#include <thread>
#endif

#include <cstddef>

#include <ncurses.h>
#include <optional/optional.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>

#ifndef add_wchstr
#include <cppurses/painter/detail/extended_char.hpp>
#endif

namespace {
using namespace cppurses;

attr_t color_attr_t(Color c) {
    return static_cast<attr_t>(c) - detail::first_color_value;
}

short find_pair(Color foreground, Color background) {
    const auto color_count = 16;
    return color_attr_t(background) * color_count + color_attr_t(foreground);
}

short find_pair(const Brush& brush) {
    auto background = Color::Black;
    if (brush.background_color()) {
        background = *(brush.background_color());
    }
    auto foreground = Color::Black;
    if (brush.foreground_color()) {
        foreground = *(brush.foreground_color());
    }
    return find_pair(foreground, background);
}

attr_t attribute_to_attr_t(Attribute attr) {
    auto result = A_NORMAL;
    switch (attr) {
        case Attribute::Bold:
            result = A_BOLD;
            break;
        case Attribute::Underline:
            result = A_UNDERLINE;
            break;
        case Attribute::Standout:
            result = A_STANDOUT;
            break;
        case Attribute::Dim:
            result = A_DIM;
            break;
        case Attribute::Inverse:
            result = A_REVERSE;
            break;
        case Attribute::Invisible:
            result = A_INVIS;
            break;
        case Attribute::Blink:
            result = A_BLINK;
            break;
        case Attribute::Italic:
#ifdef A_ITALIC
            result = A_ITALIC;
            break;
#endif
    }
    return result;
}

attr_t find_attr_t(const Brush& brush) {
    auto result = A_NORMAL;
    for (Attribute a : Attribute_list) {
        if (brush.has_attribute(a)) {
            result |= attribute_to_attr_t(a);
        }
    }
    return result;
}

#ifdef SLOW_PAINT
void paint_indicator(char symbol) {
    const auto color_pair = find_pair(Color::White, Color::Black);
    const auto attributes = A_NORMAL;
#ifdef add_wchstr
    const wchar_t temp_sym[2] = {symbol, L'\0'};
    auto temp_display = cchar_t{' '};
    ::setcchar(&temp_display, temp_sym, attributes, color_pair, nullptr);
    ::wadd_wchnstr(::stdscr, &temp_display, 1);
#else
    ::waddch(::stdscr, symbol | COLOR_PAIR(color_pair) | attributes);
#endif
    refresh();
    std::this_thread::sleep_for(std::chrono::milliseconds(SLOW_PAINT));
}
#endif

#ifdef add_wchstr
/// Add \p glyph's symbol, with attributes, to the screen at cursor position.
void put_as_wchar(const Glyph& glyph) {
    const auto color_pair = find_pair(glyph.brush);
    const auto attributes = find_attr_t(glyph.brush);
    const wchar_t symbol[2] = {glyph.symbol, L'\0'};
    auto symbol_and_attributes = cchar_t{' '};

    ::setcchar(&symbol_and_attributes, symbol, attributes, color_pair, nullptr);
    ::wadd_wchnstr(::stdscr, &symbol_and_attributes, 1);
}
#else

/// Add \p glyph's symbol, with attributes, to the screen at cursor position.
void put_as_char(const Glyph& glyph) {
    auto use_addch = false;
    auto symbol_and_attributes = detail::get_chtype(glyph.symbol, use_addch);
    symbol_and_attributes |= COLOR_PAIR(find_pair(glyph.brush));
    symbol_and_attributes |= find_attr_t(glyph.brush);
    if (use_addch) {
        ::waddch(::stdscr, symbol_and_attributes);
    } else {
        ::waddchnstr(::stdscr, &symbol_and_attributes, 1);
    }
}
#endif
}  // namespace

namespace cppurses {
namespace output {

void move_cursor(std::size_t x, std::size_t y) {
    ::wmove(::stdscr, static_cast<int>(y), static_cast<int>(x));
}

void refresh() {
    ::wrefresh(::stdscr);
}

void put(const Glyph& g) {
#ifdef SLOW_PAINT
    paint_indicator('X');
#endif
#ifdef add_wchstr
    put_as_wchar(g);
#else  // If no wchar_t support in ncurses.
    put_as_char(g);
#endif
#ifdef SLOW_PAINT
    refresh();
    std::this_thread::sleep_for(std::chrono::milliseconds(SLOW_PAINT));
#endif
}

}  // namespace output
}  // namespace cppurses
