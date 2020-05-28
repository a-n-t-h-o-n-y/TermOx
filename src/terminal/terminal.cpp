#include <cppurses/terminal/terminal.hpp>

#include <algorithm>
#include <chrono>
#include <clocale>
#include <csignal>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <mutex>
#include <stdexcept>

#include <ncurses.h>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/palette/basic.hpp>
#include <cppurses/painter/palette/basic8.hpp>
#include <cppurses/painter/palette/dawn_bringer16.hpp>
#include <cppurses/painter/rgb.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/terminal/input.hpp>
#include <cppurses/terminal/terminal_error.hpp>

extern "C" void handle_sigint(int /* sig*/)
{
    cppurses::System::terminal.uninitialize();
#if !defined __APPLE__ && !defined __MINGW32__
    std::quick_exit(0);
#else
    std::exit(0);
#endif
}

namespace {
using namespace cppurses;

auto scale(RGB::Value_t value) -> short
{
    auto constexpr value_max   = RGB::Value_t{255};
    auto constexpr ncurses_max = short{1000};
    return static_cast<short>((static_cast<double>(value) / value_max) *
                              ncurses_max);
}

}  // namespace

namespace cppurses {

void Terminal::initialize()
{
    if (is_initialized_)
        return;
    std::setlocale(LC_ALL, "en_US.UTF-8");

    if (::newterm(std::getenv("TERM"), stdout, stdin) == nullptr and
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
        catch (Terminal_error) {
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
    auto y = 0;
    auto x = 0;
    if (is_initialized_)
        getmaxyx(::stdscr, y, x);
    return x;
}

auto Terminal::height() const -> std::size_t
{
    auto y = 0;
    auto x = 0;
    if (is_initialized_)
        getmaxyx(::stdscr, y, x);
    return y;
}

void Terminal::set_refresh_rate(std::chrono::milliseconds duration)
{
    refresh_rate_ = duration;
    if (is_initialized_)
        ::timeout(refresh_rate_.count());
}

void Terminal::set_background(Glyph const& tile)
{
    background_ = tile;
    if (is_initialized_) {
        // TODO Notify Screen::flush() to repaint everything.
    }
}

void Terminal::set_palette(ANSI_palette const& colors)
{
    if (!is_initialized_ or !this->has_color())
        return;
    palette_           = colors;
    auto const color_n = this->get_ansi_color_count();
    for (auto fg = Color::Value_t{0}; fg < color_n; ++fg) {
        for (auto bg = Color::Value_t{0}; bg < color_n; ++bg) {
            ::init_pair(this->color_index(fg, bg), this->get_ansi_value(fg),
                        this->get_ansi_value(bg));
        }
    }
}

void Terminal::set_palette(True_color_palette const& colors)
{
    if (!is_initialized_)
        return;
    if (!this->has_color() || !this->can_change_colors())
        throw Terminal_error{"Terminal cannot re-define color values."};
    auto ansi_pal = ANSI_palette{};
    ansi_pal.reserve(colors.size());
    std::transform(std::cbegin(colors), std::cend(colors),
                   std::back_inserter(ansi_pal),
                   [](auto const& tcd) { return tcd.ansi_def; });
    this->set_palette(ansi_pal);
    for (auto const& color : colors)
        this->set_color_definition(color);
    tc_palette_ = colors;
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

auto Terminal::color_pair_count() const -> short
{
    if (is_initialized_)
        return COLOR_PAIRS;
    return 0;
}

auto Terminal::color_index(Color::Value_t fg, Color::Value_t bg) const -> short
{
    // Can use ansi_color_count because this is set even with true color palette
    short const color_n = this->get_ansi_color_count();
    short const offset  = 1;
    short const fg_mod  = fg % color_n;
    short const bg_mod  = bg % color_n;
    return offset + fg_mod * color_n + bg_mod;
}

void Terminal::set_color_definition(True_color_definition const& def)
{
    if (!this->can_change_colors())
        return;
    ::init_color(def.ansi_def.ansi.value, scale(def.color_value.red()),
                 scale(def.color_value.green()), scale(def.color_value.blue()));
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

auto Terminal::get_ansi_value(Color::Value_t c) -> ANSI::Value_t
{
    auto const iter = std::find_if(std::cbegin(palette_), std::cend(palette_),
                                   [c](auto x) { return x.color.value == c; });
    if (iter == std::cend(palette_))
        throw std::runtime_error{"Missing Color in Palette."};
    return iter->ansi.value;
}

}  // namespace cppurses
