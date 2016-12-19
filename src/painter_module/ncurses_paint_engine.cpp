#include <painter_module/attribute.hpp>
#include <painter_module/detail/ncurses_paint_engine.hpp>
#include <painter_module/color.hpp>

#include <ncurses.h>

#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>

namespace mcurses {
namespace detail {

NCurses_paint_engine::NCurses_paint_engine() {
    setenv("TERM", "xterm-256color", 1);
    ::setlocale(LC_ALL, "en_US.UTF-8");
    ::initscr();
    ::cbreak();  // change to raw() once you can handle exit signals on your
                 // own.
    ::noecho();
    ::keypad(::stdscr, true);
    ::mousemask(ALL_MOUSE_EVENTS, nullptr);
    ::mouseinterval(0);
    ::curs_set(0);  // invisible cursor
    ::start_color();

    this->buffer_.resize(this->screen_width(), this->screen_height());
    this->initialize_color_pairs();
}
NCurses_paint_engine::~NCurses_paint_engine() {
    ::endwin();
}

void NCurses_paint_engine::show_cursor() {
    ::curs_set(1);
}
void NCurses_paint_engine::hide_cursor() {
    ::curs_set(0);
}

unsigned NCurses_paint_engine::screen_width() {
    int x{0}, y{0};
    getmaxyx(stdscr, y, x);
    buffer_.resize(x, y);  // not the best, but it works
    return x;
}

unsigned NCurses_paint_engine::screen_height() {
    int x{0}, y{0};
    getmaxyx(stdscr, y, x);
    buffer_.resize(x, y);  // not the best, but it works
    return y;
}

void NCurses_paint_engine::set_rgb(Color c, int r, int g, int b) {
    auto scale = [](int i) { return (static_cast<double>(i) / 255) * 1000; };
    int r_ = scale(r);
    int g_ = scale(g);
    int b_ = scale(b);
    ::init_color(translate_(c), r_, g_, b_);
}

int NCurses_paint_engine::attr_to_int(Attribute attr) {
    int a = A_NORMAL;
    switch (attr) {
        case Attribute::Bold:
            a = A_BOLD;
            break;

        case Attribute::Italic:
            a = A_ITALIC;
            break;

        case Attribute::Underline:
            a = A_UNDERLINE;
            break;

        case Attribute::Standout:
            a = A_STANDOUT;
            break;

        case Attribute::Dim:
            a = A_DIM;
            break;

        case Attribute::Inverse:
            a = A_REVERSE;
            break;

        case Attribute::Invisible:
            a = A_INVIS;
            break;

        case Attribute::Blink:
            a = A_BLINK;
            break;
    }
    return a;
}

void NCurses_paint_engine::move(unsigned x, unsigned y) {
    ::wmove(::stdscr, y, x);
}

void NCurses_paint_engine::put_string(const std::string& s) {
    ::waddstr(::stdscr, s.c_str());
}

void NCurses_paint_engine::clear_attributes() {
    ::wattron(::stdscr, A_NORMAL);
}

void NCurses_paint_engine::set_attribute(Attribute attr) {
    ::wattron(::stdscr, attr_to_int(attr));
}

void NCurses_paint_engine::set_background_color(Color c) {
    ::wattron(::stdscr,
              COLOR_PAIR(this->find_pair(this->current_foreground(), c)));
}

void NCurses_paint_engine::set_foreground_color(Color c) {
    ::wattron(::stdscr,
              COLOR_PAIR(this->find_pair(c, this->current_background())));
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

void NCurses_paint_engine::initialize_color_pairs() const {
    std::vector<int> int_colors = {240, 241, 242, 243, 244, 245, 246, 247,
                                   248, 249, 250, 251, 252, 253, 254, 255};
    int index{0};
    for (int i{0}; i < 16; ++i) {
        for (int j{0}; j < 16; ++j) {
            if (i == 0 && j == 0) {
                ::init_pair(index++, int_colors[15], int_colors[0]);
            } else if (i == 0 && j == 15) {
                ::init_pair(index++, int_colors[0], int_colors[15]);
            } else {
                ::init_pair(index++, int_colors[i], int_colors[j]);
            }
        }
    }
}

}  // namespace detail
}  // namespace mcurses
