#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/detail/ncurses_paint_engine.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/paint_buffer.hpp>

#include <ncurses.h>
#include <optional/optional.hpp>

#include <clocale>
#include <cstddef>
#include <cstdint>
#include <string>

namespace {

std::int16_t color_to_int(cppurses::Color c) {
    return static_cast<std::int16_t>(c) - cppurses::detail::k_init_color;
}

std::int16_t find_pair(cppurses::Color foreground, cppurses::Color background) {
    return color_to_int(background) * 16 + color_to_int(foreground);
}

std::uint32_t attr_to_int(cppurses::Attribute attr) {
    std::uint32_t a = A_NORMAL;
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

NCurses_paint_engine::NCurses_paint_engine(const Paint_buffer& buffer)
    : buffer_{buffer} {
    setenv("TERM", "xterm-256color", 1);
    std::setlocale(LC_ALL, "en_US.UTF-8");
    ::initscr();
    ::noecho();
    ::keypad(::stdscr, true);
    ::mousemask(ALL_MOUSE_EVENTS, nullptr);
    ::mouseinterval(0);
    ::start_color();
    ::assume_default_colors(k_init_color, k_init_color);
    ::ESCDELAY = 1;
    initialize_color_pairs();
    this->hide_cursor();
}

NCurses_paint_engine::~NCurses_paint_engine() {
    ::endwin();
}

void NCurses_paint_engine::put_glyph(const Glyph& g) {
    for (const Attribute& attr : g.brush().attributes()) {
        this->set_attribute(attr);
    }
    if (g.brush().background_color()) {
        this->set_background_color(*g.brush().background_color());
    }
    if (g.brush().foreground_color()) {
        this->set_foreground_color(*g.brush().foreground_color());
    }
    this->put_string(g.c_str());
    this->clear_attributes();
}

void NCurses_paint_engine::put(std::size_t x, std::size_t y, const Glyph& g) {
    this->move(x, y);
    this->put_glyph(g);
}

void NCurses_paint_engine::touch_all() {
    ::touchwin(::stdscr);
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
    int x{0}, y{0};
    getmaxyx(stdscr, y, x);
    return x;
}

std::size_t NCurses_paint_engine::screen_height() {
    int x{0}, y{0};
    getmaxyx(stdscr, y, x);
    return y;
}

void NCurses_paint_engine::set_rgb(Color c,
                                   std::int16_t r,
                                   std::int16_t g,
                                   std::int16_t b) {
    auto scale = [](std::int16_t i) {
        return (static_cast<double>(i) / 255) * 999;
    };
    std::int16_t r_ = scale(r);
    std::int16_t g_ = scale(g);
    std::int16_t b_ = scale(b);
    ::init_color(static_cast<std::int16_t>(c), r_, g_, b_);
}

void NCurses_paint_engine::move(std::size_t x, std::size_t y) {
    ::wmove(::stdscr, y, x);
}

void NCurses_paint_engine::put_string(const char* s) {
    ::waddstr(::stdscr, s);
}

void NCurses_paint_engine::put_string(const std::string& s) {
    this->put_string(s.c_str());
}

void NCurses_paint_engine::clear_attributes() {
    wattrset(::stdscr, A_NORMAL);
}

void NCurses_paint_engine::set_attribute(Attribute attr) {
    ::wattron(::stdscr, attr_to_int(attr));
}

void NCurses_paint_engine::set_background_color(Color c) {
    ::color_set(find_pair(this->current_foreground(), c), nullptr);
}

void NCurses_paint_engine::set_foreground_color(Color c) {
    ::color_set(find_pair(c, this->current_background()), nullptr);
}

void NCurses_paint_engine::refresh() {
    ::wrefresh(::stdscr);
}

Color NCurses_paint_engine::current_background() {
    int y{0};
    int x{0};
    getyx(::stdscr, y, x);
    return *buffer_.at(x, y).brush().background_color();
}

Color NCurses_paint_engine::current_foreground() {
    int y{0};
    int x{0};
    getyx(::stdscr, y, x);
    return *buffer_.at(x, y).brush().foreground_color();
}

}  // namespace detail
}  // namespace cppurses
