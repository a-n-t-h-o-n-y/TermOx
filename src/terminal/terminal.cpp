#include <cppurses/terminal/terminal.hpp>

#include <clocale>
#include <csignal>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#include <ncurses.h>
#include <signal.h>
#include <string.h>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/color_definition.hpp>
#include <cppurses/painter/palette.hpp>
#include <cppurses/painter/rgb.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/terminal/input.hpp>

extern "C" void handle_sigint(int /* sig*/) {
    cppurses::System::terminal.uninitialize();
#if !defined __APPLE__
  std::quick_exit(0);
#else
  std::exit(0);
#endif
}

namespace {
using namespace cppurses;

/// Returns the grayscale value to be used for RGB values for color number \p c.
Underlying_color_t grayscale_value(Underlying_color_t c) {
    return (c - 232) * 10 + 8;
}

Underlying_color_t scale(Underlying_color_t value) {
    const auto value_max = 255;
    const auto ncurses_max = 1000;
    return static_cast<Underlying_color_t>(
        (static_cast<double>(value) / value_max) * ncurses_max);
}

void initialize_color_pairs() {
    short pair_n = 0;
    for (auto bg = detail::first_color_value; bg < detail::last_color_value;
         ++bg) {
        for (auto fg = detail::first_color_value; fg < detail::last_color_value;
             ++fg) {
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
    std::signal(SIGINT, &handle_sigint);
    this->setup_resize_signal_handler();
    std::setlocale(LC_ALL, "en_US.UTF-8");

    if (::newterm(std::getenv("TERM"), stdout, stdin) == nullptr) {
        if (::newterm("xterm-256color", stdout, stdin) == nullptr) {
            throw std::runtime_error{"Unable to initialize screen."};
        }
    }

    is_initialized_ = true;
    ::noecho();
    ::keypad(::stdscr, true);
    ::ESCDELAY = 1;
    ::mousemask(ALL_MOUSE_EVENTS, nullptr);
    ::mouseinterval(0);
    if (this->has_color()) {
        ::start_color();
        ::assume_default_colors(detail::first_color_value,
                                detail::first_color_value);
        initialize_color_pairs();
        this->ncurses_set_palette(palette_);
    }
    this->ncurses_set_raw_mode();
    this->ncurses_set_cursor();
}

void Terminal::uninitialize() {
    if (!is_initialized_) {
        return;
    }
    this->reset_palette();
    ::wrefresh(::stdscr);
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

void Terminal::set_color_palette(const Palette& colors) {
    palette_ = colors;
    if (is_initialized_ && this->has_color()) {
        this->ncurses_set_palette(palette_);
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

void Terminal::ncurses_set_palette(const Palette& colors) {
    for (const Color_definition& def : colors) {
        Underlying_color_t ncurses_color_number{
            static_cast<Underlying_color_t>(def.color)};
        ::init_color(ncurses_color_number, scale(def.values.red),
                     scale(def.values.green), scale(def.values.blue));
    }
}

void Terminal::reset_palette() {
    Palette initial_palette;
    for (auto c = detail::first_color_value; c != detail::last_color_value;
         ++c) {
        auto value = grayscale_value(c);
        Color_definition def{static_cast<Color>(c), {value, value, value}};
        initial_palette[c - detail::first_color_value] = def;
    }
    this->ncurses_set_palette(initial_palette);
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
