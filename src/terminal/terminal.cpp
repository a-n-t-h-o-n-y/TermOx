#include <termox/terminal/terminal.hpp>

#include <algorithm>
#include <chrono>
#include <clocale>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <mutex>
#include <stdexcept>

#ifndef _XOPEN_SOURCE_EXTENDED
#    define _XOPEN_SOURCE_EXTENDED
#endif
#include <ncursesw/ncurses.h>
#undef border

#include <termox/painter/color.hpp>
#include <termox/painter/palette/basic.hpp>
#include <termox/painter/palette/basic8.hpp>
#include <termox/painter/palette/dawn_bringer16.hpp>
#include <termox/system/system.hpp>
#include <termox/terminal/input.hpp>
#include <termox/terminal/terminal_error.hpp>
#include <termox/widget/widget.hpp>

extern "C" void handle_sigint(int /* sig*/)
{
    ox::System::terminal.uninitialize();
#if !defined __APPLE__ && !defined __MINGW32__
    std::quick_exit(0);
#else
    std::exit(0);
#endif
}

namespace {
using namespace ox;

auto scale(RGB::Value_t value) -> short
{
    auto constexpr value_max   = RGB::Value_t{255};
    auto constexpr ncurses_max = short{1000};
    return static_cast<short>((static_cast<double>(value) / value_max) *
                              ncurses_max);
}

}  // namespace

namespace ox {

void Terminal::initialize()
{
    if (is_initialized_)
        return;
    std::setlocale(LC_ALL, "en_US.UTF-8");

    if (::newterm(std::getenv("TERM"), stdout, stdin) == nullptr &&
        ::newterm("xterm-256color", stdout, stdin) == nullptr) {
        throw std::runtime_error{"Unable to initialize screen."};
    }
    std::signal(SIGINT, &handle_sigint);

    is_initialized_ = true;
    ::noecho();
    ::keypad(::stdscr, true);
    ::set_escdelay(1);
    ::mousemask(ALL_MOUSE_EVENTS, nullptr);
    ::mouseinterval(0);
    this->set_refresh_rate(refresh_rate_);
    if (this->has_color()) {
        ::start_color();
        try {
            this->set_palette(dawn_bringer16::palette);
        }
        catch (Terminal_error const&) {
            if (this->has_extended_colors())
                this->set_palette(basic::palette);
            else
                this->set_palette(basic8::palette);
        }
    }
    this->ncurses_set_raw_mode();
    this->ncurses_set_cursor();
}

void Terminal::uninitialize()
{
    if (!is_initialized_)
        return;
    ::wrefresh(::stdscr);
    is_initialized_ = false;
    ::endwin();
}

// getmaxx/getmaxy are non-standard.
auto Terminal::width() const -> std::size_t
{
    auto x = 0;
    if (auto y = 0; is_initialized_)
        getmaxyx(::stdscr, y, x);
    return x;
}

auto Terminal::height() const -> std::size_t
{
    auto y = 0;
    if (auto x = 0; is_initialized_)
        getmaxyx(::stdscr, y, x);
    return y;
}

void Terminal::set_refresh_rate(std::chrono::milliseconds duration)
{
    refresh_rate_ = duration;
    if (is_initialized_)
        ::timeout(refresh_rate_.count());
}

void Terminal::set_background(Glyph tile)
{
    background_ = tile;
    if (is_initialized_)
        this->repaint_all();
}

void Terminal::set_palette(Palette colors)
{
    if (!is_initialized_ || !this->has_color())
        return;
    dynamic_color_engine_.clear();
    palette_ = std::move(colors);
    for (auto const& def : palette_) {
        std::visit(
            [&](auto const& d) {
                this->set_color_definition(def.color, def.ansi, d);
            },
            def.value);
    }
    this->repaint_all();
    palette_changed(palette_);
}

auto Terminal::palette_append(Color_definition::Value_t value) -> Color
{
    auto pal = this->current_palette();
    if (pal.empty())
        pal.push_back({Color{0}, ANSI{16}, value});
    else {
        pal.push_back(
            {Color{static_cast<Color::Value_t>(pal.back().color.value + 1)},
             ANSI{static_cast<ANSI::Value_t>(pal.back().ansi.value + 1)},
             value});
    }
    this->set_palette(pal);
    return pal.back().color;
}

void Terminal::initialize_pairs(Color c, ANSI a)
{
    for (auto const& def : palette_) {
        ::init_pair(this->color_index(c, def.color), a.value, def.ansi.value);
        ::init_pair(this->color_index(def.color, c), def.ansi.value, a.value);
    }
}

void Terminal::set_color_definition(Color c, ANSI a, std::monostate)
{
    this->initialize_pairs(c, a);
}

void Terminal::set_color_definition(Color c, ANSI a, True_color value)
{
    this->initialize_pairs(c, a);
    this->term_set_color(a, value);
}

void Terminal::set_color_definition(Color c, ANSI a, Dynamic_color value)
{
    this->initialize_pairs(c, a);
    dynamic_color_engine_.register_color(a, value);
}

auto Terminal::get_ansi(Color c) -> short
{
    auto const iter =
        std::find_if(std::begin(palette_), std::end(palette_),
                     [c](auto const& def) { return def.color == c; });
    if (iter != std::end(palette_))
        return iter->ansi.value;
    throw std::runtime_error("get_ansi: Color not found in palette.");
}

auto Terminal::color_content(ANSI ansi) -> RGB
{
    short r, g, b;
    ::color_content(ansi.value, &r, &g, &b);
    auto const scale_to_256 = [](short x) -> std::uint8_t {
        return x / 1000. * 256;
    };
    return {
        scale_to_256(r),
        scale_to_256(b),
        scale_to_256(b),
    };
}

void Terminal::show_cursor(bool show)
{
    show_cursor_ = show;
    if (is_initialized_)
        this->ncurses_set_cursor();
}

void Terminal::raw_mode(bool enable)
{
    raw_mode_ = enable;
    if (is_initialized_)
        this->ncurses_set_raw_mode();
}

auto Terminal::has_color() const -> bool
{
    if (is_initialized_)
        return ::has_colors() == TRUE;
    return false;
}

auto Terminal::has_extended_colors() const -> bool
{
    if (is_initialized_)
        return COLORS >= 16;
    return false;
}

auto Terminal::color_count() const -> short
{
    if (is_initialized_)
        return COLORS;
    return 0;
}

auto Terminal::can_change_colors() const -> bool
{
    if (is_initialized_)
        return ::can_change_color() == TRUE;
    return false;
}

auto Terminal::color_pair_count() const -> int
{
    if (is_initialized_)
        return COLOR_PAIRS;
    return 0;
}

auto Terminal::color_index(Color fg, Color bg) const -> short
{
    auto constexpr color_count = 181uL;
    return 1 + (fg.value * color_count) + bg.value;
}

void Terminal::term_set_color(ANSI a, True_color value)
{
    if (!this->can_change_colors())
        throw Terminal_error{"Terminal cannot re-define color values."};

    ::init_color(a.value, scale(value.red()), scale(value.green()),
                 scale(value.blue()));
}

void Terminal::ncurses_set_raw_mode() const
{
    if (raw_mode_) {
        ::nocbreak();
        ::raw();
    }
    else {
        ::noraw();
        ::cbreak();
    }
}

void Terminal::ncurses_set_cursor() const
{
    show_cursor_ ? ::curs_set(1) : ::curs_set(0);
}

void Terminal::repaint_all()
{
    auto* const head = System::head();
    if (head == nullptr)
        return;
    for (auto* const d : head->get_descendants())
        d->update();
}

}  // namespace ox
