#include <cppurses/painter/detail/ncurses_paint_engine.hpp>

#include <clocale>
#include <cstddef>
#include <cstdint>

#include <ncurses.h>
#include <optional/optional.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#if NCURSES_VERSION_MAJOR < 6
#include <cppurses/painter/detail/extended_char.hpp>
#endif

namespace {
attr_t color_to_int(cppurses::Color c) {
    return static_cast<attr_t>(c) - cppurses::detail::k_init_color;
}

attr_t find_pair(cppurses::Color foreground, cppurses::Color background) {
    const int color_n{16};
    return color_to_int(background) * color_n + color_to_int(foreground);
}

attr_t attr_to_int(cppurses::Attribute attr) {
    attr_t a = A_NORMAL;
    switch (attr) {
        case cppurses::Attribute::Bold:
            a = A_BOLD;
            break;
        case cppurses::Attribute::Italic:
#if defined(A_ITALIC)
            a = A_ITALIC;
#endif
            break;
        case cppurses::Attribute::Underline:
            a = A_UNDERLINE;
            break;
        case cppurses::Attribute::Standout:
            a = A_STANDOUT;
            break;
        case cppurses::Attribute::Dim:
            a = A_DIM;
            break;
        case cppurses::Attribute::Inverse:
            a = A_REVERSE;
            break;
        case cppurses::Attribute::Invisible:
            a = A_INVIS;
            break;
        case cppurses::Attribute::Blink:
            a = A_BLINK;
            break;
    }
    return a;
}

void initialize_color_pairs() {
    int pair{0};
    const int end_color{cppurses::detail::k_init_color + 16};
    for (int i{cppurses::detail::k_init_color}; i < end_color; ++i) {
        for (int j{cppurses::detail::k_init_color}; j < end_color; ++j) {
            if (pair != 0) {
                ::init_pair(pair, j, i);
            }
            ++pair;
        }
    }
}

}  // namespace

namespace cppurses {
namespace detail {

NCurses_paint_engine::NCurses_paint_engine() {
    setenv("TERM", "xterm-256color", 1);
    std::setlocale(LC_ALL, "en_US.UTF-8");

    ::initscr();
    ::noecho();
    ::keypad(::stdscr, true);
    ::ESCDELAY = 1;
    ::mousemask(ALL_MOUSE_EVENTS, nullptr);
    ::mouseinterval(0);
    ::start_color();
    ::assume_default_colors(k_init_color, k_init_color);
    initialize_color_pairs();
    this->hide_cursor();
}

NCurses_paint_engine::~NCurses_paint_engine() {
    ::endwin();
}

void NCurses_paint_engine::put_glyph(const Glyph& g) {
    // Background Color
    Color back_color{Color::Black};
    if (g.brush.background_color()) {
        back_color = *g.brush.background_color();
    }

    // Foreground Color
    Color fore_color{Color::Black};
    if (g.brush.foreground_color()) {
        fore_color = *g.brush.foreground_color();
    }
    attr_t color_pair{COLOR_PAIR(find_pair(fore_color, back_color))};

    // Attributes
    attr_t attributes{0};
    for (const Attribute& attr : g.brush.attributes()) {
        attributes |= attr_to_int(attr);
    }

#if NCURSES_VERSION_MAJOR >= 6  // wchar_t support
    cchar_t image{0, {g.symbol}};
    image.attr |= color_pair;
    image.attr |= attributes;
    ::wadd_wchnstr(::stdscr, &image, 1);

#else  // no wchar_t support
    bool use_addch{false};
    chtype image{find_chtype(g.symbol, &use_addch)};
    image |= color_pair;
    image |= attributes;
    if (use_addch) {  // For extended chars
        ::waddch(::stdscr, image);
    } else {
        ::waddchnstr(::stdscr, &image, 1);
    }
#endif
}

void NCurses_paint_engine::put(std::size_t x, std::size_t y, const Glyph& g) {
    this->move(x, y);
    this->put_glyph(g);
}

void NCurses_paint_engine::move(std::size_t x, std::size_t y) {
    ::wmove(::stdscr, static_cast<int>(y), static_cast<int>(x));
}

void NCurses_paint_engine::show_cursor(bool show) {
    if (show) {
        ::curs_set(1);
    } else {
        ::curs_set(0);
    }
}
void NCurses_paint_engine::hide_cursor(bool hide) {
    this->show_cursor(!hide);
}

std::size_t NCurses_paint_engine::screen_width() {
    return getmaxx(::stdscr);
}

std::size_t NCurses_paint_engine::screen_height() {
    return getmaxy(::stdscr);
}

void NCurses_paint_engine::touch_all() {
    ::touchwin(::stdscr);
}

void NCurses_paint_engine::refresh() {
    ::wrefresh(::stdscr);
}

void NCurses_paint_engine::set_rgb(Color c,
                                   std::int16_t r,
                                   std::int16_t g,
                                   std::int16_t b) {
    auto scale = [](std::int16_t i) {
        return static_cast<std::int16_t>((static_cast<double>(i) / 255) * 999);
    };
    std::int16_t r_{scale(r)};
    std::int16_t g_{scale(g)};
    std::int16_t b_{scale(b)};
    ::init_color(static_cast<std::int16_t>(c), r_, g_, b_);
}

}  // namespace detail
}  // namespace cppurses
