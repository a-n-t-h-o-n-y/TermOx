#include <cppurses/painter/detail/ncurses_paint_engine.hpp>

#include <array>
#include <clocale>
#include <cstddef>
#include <stdexcept>

#include <signal.h>
#include <string.h>

#include <ncurses.h>

#include <optional/optional.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/detail/ncurses_data.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/system/system.hpp>

#ifndef add_wchstr
#include <cppurses/painter/detail/extended_char.hpp>
#endif

// #define SLOW_PAINT 7

#if defined(SLOW_PAINT)
#include <chrono>
#include <thread>
#endif

namespace {
using namespace cppurses;

attr_t color_value(Color c) {
    return static_cast<attr_t>(c) - detail::first_color_value;
}

short find_pair(Color foreground, Color background) {
    const auto color_count = 16;
    return color_value(background) * color_count + color_value(foreground);
}

short find_pair(const Brush& brush) {
    auto bg_color = Color::Black;
    if (brush.background_color()) {
        bg_color = *(brush.background_color());
    }
    auto fg_color = Color::Black;
    if (brush.foreground_color()) {
        fg_color = *(brush.foreground_color());
    }
    return find_pair(fg_color, bg_color);
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
#if defined(A_ITALIC)
            result = A_ITALIC;
#endif
            break;
        default:
            throw std::domain_error{"Attribute enum not handled."};
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

void initialize_color_pairs() {
    using detail::first_color_value;
    const auto last_color_value = first_color_value + 16;
    short pair_n = 0;
    for (auto bg = first_color_value; bg < last_color_value; ++bg) {
        for (auto fg = first_color_value; fg < last_color_value; ++fg) {
            if (pair_n != 0) {
                ::init_pair(pair_n, fg, bg);
            }
            ++pair_n;
        }
    }
}

#if defined(add_wchstr)
/// Adds \p glyph's symbol, with attributes, to the screen at cursor position.
void add_as_wchar(const Glyph& glyph) {
    const auto color_pair = find_pair(glyph.brush);
    const auto attributes = find_attr_t(glyph.brush);
    const wchar_t symbol[2] = {glyph.symbol, L'\0'};

    auto symbol_and_attributes = cchar_t{' '};
    ::setcchar(&symbol_and_attributes, symbol, attributes, color_pair, nullptr);
    ::wadd_wchnstr(::stdscr, &symbol_and_attributes, 1);
}
#else
/// Adds \p glyph's symbol, with attributes, to the screen at cursor position.
void add_as_char(const Glyph& glyph) {
    auto use_addch = false;
    auto symbol_and_attributes = detail::get_chtype(glyph.symbol, use_addch);
    symbol_and_attributes |= find_pair(glyph.brush);
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
namespace detail {

NCurses_paint_engine::NCurses_paint_engine() {
    setenv("TERM", "xterm-256color", 1);  // TODO not a great idea.
    std::setlocale(LC_ALL, "en_US.UTF-8");
    this->setup_sigwinch();
    ::initscr();
    ::noecho();
    ::keypad(::stdscr, true);
    ::ESCDELAY = 1;
    ::mousemask(ALL_MOUSE_EVENTS, nullptr);
    ::mouseinterval(0);
    ::start_color();
    ::assume_default_colors(first_color_value, first_color_value);
    initialize_color_pairs();
    this->hide_cursor();
}

NCurses_paint_engine::~NCurses_paint_engine() {
    ::endwin();
}

void NCurses_paint_engine::put_glyph(const Glyph& g) {
#if defined(SLOW_PAINT)
    paint_temporary(L'X');
#endif
#if defined(add_wchstr)
    add_as_wchar(g);
#else
    // If no wchar_t support in ncurses.
    add_as_char(g);
#endif
#if defined(SLOW_PAINT)
    this->refresh();
    std::this_thread::sleep_for(std::chrono::milliseconds(SLOW_PAINT));
#endif
}

void NCurses_paint_engine::move_cursor(std::size_t x, std::size_t y) {
    ::wmove(::stdscr, static_cast<int>(y), static_cast<int>(x));
}

void NCurses_paint_engine::show_cursor(bool show) {
    show ? ::curs_set(1) : ::curs_set(0);
}

void NCurses_paint_engine::refresh() {
    ::wrefresh(::stdscr);
}

void handle_sigwinch(int sig) {
    NCurses_data::resize_happened = true;
}

void NCurses_paint_engine::setup_sigwinch() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = handle_sigwinch;
    sigaction(SIGWINCH, &sa, NULL);
}

void NCurses_paint_engine::paint_temporary(wchar_t display) {
    const wchar_t temp_sym[2] = {display, L'\0'};
    const auto temp_color_pair = find_pair(Color::White, Color::Black);
    auto temp_display = cchar_t{' '};
    ::setcchar(&temp_display, temp_sym, A_NORMAL, temp_color_pair, nullptr);
    ::wadd_wchnstr(::stdscr, &temp_display, 1);
    this->refresh();
#if defined(SLOW_PAINT)
    std::this_thread::sleep_for(std::chrono::milliseconds(SLOW_PAINT));
#endif
}

}  // namespace detail
}  // namespace cppurses
