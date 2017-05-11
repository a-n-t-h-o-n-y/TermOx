#include "painter_module/detail/ncurses_paint_engine.hpp"
#include "painter_module/attribute.hpp"
#include "painter_module/color.hpp"
#include <ncurses.h>
#include <string>
#include <cstddef>
#include <cstdint>

// local namespace
namespace {

std::int16_t color_to_int(twf::Color c) {
    return static_cast<std::int16_t>(c) - 240;
}

std::int16_t find_pair(twf::Color foreground, twf::Color background) {
    return color_to_int(background) * 16 + color_to_int(foreground);
}

std::uint32_t attr_to_int(twf::Attribute attr) {
    std::uint32_t a = A_NORMAL;
    switch (attr) {
        case twf::Attribute::Bold:
            a = A_BOLD;
            break;

        case twf::Attribute::Italic:
            a = A_ITALIC;
            break;

        case twf::Attribute::Underline:
            a = A_UNDERLINE;
            break;

        case twf::Attribute::Standout:
            a = A_STANDOUT;
            break;

        case twf::Attribute::Dim:
            a = A_DIM;
            break;

        case twf::Attribute::Inverse:
            a = A_REVERSE;
            break;

        case twf::Attribute::Invisible:
            a = A_INVIS;
            break;

        case twf::Attribute::Blink:
            a = A_BLINK;
            break;
    }
    return a;
}

void initialize_color_pairs() {
    int index{0};
    for (int i{240}; i < 256; ++i) {
        for (int j{240}; j < 256; ++j) {
            ::init_pair(index++, j, i);
        }
    }
}
}  // namespace

namespace twf {
namespace detail {

NCurses_paint_engine::NCurses_paint_engine() {
    setenv("TERM", "xterm-256color", 1);
    ::setlocale(LC_ALL, "en_US.UTF-8");
    ::initscr();
    ::raw();
    ::noecho();
    ::keypad(::stdscr, true);
    ::mousemask(ALL_MOUSE_EVENTS, nullptr);
    ::mouseinterval(0);
    ::start_color();
    ::assume_default_colors(240, 240);  // Sets color pair 0 to black/black
    buffer_.resize(NCurses_paint_engine::screen_width(),
                   NCurses_paint_engine::screen_height());
    initialize_color_pairs();
}

NCurses_paint_engine::~NCurses_paint_engine() {
    ::endwin();
}

void NCurses_paint_engine::touch_all() {
    ::touchwin(::stdscr);
}

void NCurses_paint_engine::show_cursor() {
    ::curs_set(1);
}
void NCurses_paint_engine::hide_cursor() {
    ::curs_set(0);
}

std::size_t NCurses_paint_engine::screen_width() {
    int x{0}, y{0};
    getmaxyx(stdscr, y, x);
    buffer_.resize(x, y);
    return x;
}

std::size_t NCurses_paint_engine::screen_height() {
    int x{0}, y{0};
    getmaxyx(stdscr, y, x);
    buffer_.resize(x, y);
    return y;
}

void NCurses_paint_engine::set_rgb(Color c,
                                   std::uint8_t r,
                                   std::uint8_t g,
                                   std::uint8_t b) {
    auto scale = [](std::uint8_t i) {
        return (static_cast<double>(i) / 255) * 1000;
    };
    std::uint8_t r_ = scale(r);
    std::uint8_t g_ = scale(g);
    std::uint8_t b_ = scale(b);
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
    ::wattron(::stdscr, A_NORMAL);
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
}  // namespace twf
