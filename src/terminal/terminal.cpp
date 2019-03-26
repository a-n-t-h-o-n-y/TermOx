#include <cppurses/terminal/terminal.hpp>

#include <cstddef>
#include <cstdint>

#include <ncurses.h>
#include <signal.h>
#include <string.h>

#include <cppurses/painter/color_definition.hpp>
#include <cppurses/painter/palette.hpp>
#include <cppurses/terminal/input.hpp>

#include <clocale>

namespace {
std::int16_t scale(std::int16_t value) {
    const auto value_max = 255;
    const auto ncurses_max = 999;
    return static_cast<std::int16_t>((static_cast<double>(value) / value_max) *
                                     ncurses_max);
}

void initialize_color_pairs() {
    using cppurses::detail::first_color_value;
    const auto color_count = 16;
    const auto last_color_value = first_color_value + color_count;
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
}  // namespace

namespace cppurses {

void Terminal::initialize() {
    if (is_initialized_) {
        return;
    }
    setenv("TERM", "xterm-256color", 1);
    std::setlocale(LC_ALL, "en_US.UTF-8");
    this->setup_resize_signal_handler();

    ::initscr();
    is_initialized_ = true;
    ::noecho();
    ::keypad(::stdscr, true);
    ::ESCDELAY = 1;
    ::mousemask(ALL_MOUSE_EVENTS, nullptr);
    ::mouseinterval(0);
    if (this->has_color()) {
        ::start_color();
        using detail::first_color_value;
        ::assume_default_colors(first_color_value, first_color_value);
        initialize_color_pairs();
    }
    this->ncurses_set_palette();
    this->ncurses_set_raw_mode();
    this->ncurses_set_cursor();
}

void Terminal::uninitialize() {
    if (!is_initialized_) {
        return;
    }
    is_initialized_ = false;
    ::endwin();
}

// getmaxx/getmaxy are non-standard.
std::size_t Terminal::width() const {
    int y{0};
    int x{0};
    if (is_initialized_) {
        getmaxyx(::stdscr, y, x);
    }
    return x;
}

std::size_t Terminal::height() const {
    int y{0};
    int x{0};
    if (is_initialized_) {
        getmaxyx(::stdscr, y, x);
    }
    return y;
}

void Terminal::resize(std::size_t width, std::size_t height) {
    if (is_initialized_) {
        ::resizeterm(height, width);  // glitch here w/multi-thread?
    }
}

void Terminal::set_background(const Glyph& tile) {
    background_ = tile;
    if (is_initialized_) {
        // TODO Notify Screen::flush() to repaint everything.
    }
}

void Terminal::set_color_palette(const Palette& palette) {
    palette_ = palette;
    if (is_initialized_) {
        this->ncurses_set_palette();
    }
}

void Terminal::show_cursor(bool show) {
    show_cursor_ = show;
    if (is_initialized_) {
        this->ncurses_set_cursor();
    }
}

void Terminal::raw_mode(bool enable) {
    raw_mode_ = enable;
    if (is_initialized_) {
        this->ncurses_set_raw_mode();
    }
}

bool Terminal::has_color() const {
    if (is_initialized_) {
        return ::has_colors() == TRUE;
    }
    return false;
}

bool Terminal::can_change_colors() const {
    if (is_initialized_) {
        return ::can_change_color() == TRUE;
    }
    return false;
}

void Terminal::setup_resize_signal_handler() const {
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = input::indicate_resize;
    sigaction(SIGWINCH, &sa, NULL);
}

void Terminal::ncurses_set_palette() const {
    if (!this->can_change_colors()) {
        return;
    }
    for (const Color_definition& def : palette_) {
        std::int16_t ncurses_color_number{static_cast<std::int16_t>(def.color)};
        ::init_color(ncurses_color_number, scale(def.values.red),
                     scale(def.values.green), scale(def.values.blue));
    }
}

void Terminal::ncurses_set_raw_mode() const {
    if (raw_mode_) {
        ::nocbreak();
        ::raw();
    } else {
        ::noraw();
        ::cbreak();
    }
}

void Terminal::ncurses_set_cursor() const {
    show_cursor_ ? ::curs_set(1) : ::curs_set(0);
}

}  // namespace cppurses
