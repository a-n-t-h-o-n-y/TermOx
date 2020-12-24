#ifndef TERMOX_TERMINAL_TERMINAL_HPP
#define TERMOX_TERMINAL_TERMINAL_HPP
#include <chrono>
#include <cstddef>

#include <signals_light/signal.hpp>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/terminal/dynamic_color_engine.hpp>

namespace ox {

class Terminal {
   public:
    sl::Signal<void(Palette const&)> palette_changed;

   public:
    /// Initializes the terminal screen into curses mode.
    /** Must be called before any input/output can occur. Also initializes
     *  various properties that are modifiable from this Terminal class. No-op
     *  if already initialized. */
    void initialize();

    /// Reset the terminal to its state before initialize() was called.
    /** No-op if already uninitialized. */
    void uninitialize();

    /// Return the Area of the terminal screen.
    auto area() const -> Area { return {this->width(), this->height()}; }

    /// Return the width of the terminal screen.
    auto width() const -> std::size_t;

    /// Return the height of the terminal screen.
    auto height() const -> std::size_t;

    /// Set the rate at which the screen will update.
    /** User input will immediately flush, but other event loops are only
     *  processed every refresh rate. Default is 33ms. */
    void set_refresh_rate(std::chrono::milliseconds duration);

    /// Set the default background/wallpaper tiles to be used.
    /** This is used if a Widget has no assigned wallpaper. */
    void set_background(Glyph tile);

    /// Return the default background/wallpaper currently in use.
    auto background() const -> Glyph { return background_; }

    /// Change Color definitions.
    void set_palette(Palette colors);

    /// Append a Color_definition::Value_t to the current color palette.
    /** Returns the Color that \p def was paired with. Picks the Color by
     *  incrementing the last color in the current palette. */
    auto palette_append(Color_definition::Value_t value) -> Color;

    /// Return a copy of the currently set ANSI color palette.
    auto current_palette() const -> Palette const& { return palette_; }

    /// Set a single ANSI Color value.
    void set_color_definition(Color c, ANSI a, std::monostate);

    /// Set a single True_color value.
    void set_color_definition(Color c, ANSI a, True_color value);

    /// Set a single Dynamic_color value.
    void set_color_definition(Color c, ANSI a, Dynamic_color value);

    auto get_ansi(Color c) -> short;

    /// Retrieve the RGB values of a given ANSI color.
    auto color_content(ANSI c) -> RGB;

    /// Lock a Color and ANSI value together, with all possible combinations.
    /** Relies on the palette_ objects being accurate. */
    void initialize_pairs(Color c, ANSI a);

    /// Set the RGB value of an ANSI color.
    void term_set_color(ANSI a, True_color value);

    /// Set whether or not the cursor is visible on screen.
    void show_cursor(bool show = true);

    /// Enable or disable raw mode.
    /** In raw mode, the interrupt, quit, suspend, and flow control characters
     *  are all passed through uninterpreted, instead of generating a signal.
     *  This mode is off by default. */
    void raw_mode(bool enable = true);

    /// Return whether this terminal can display colors.
    /** Return false if the Terminal hasn't been initialized yet. */
    auto has_color() const -> bool;

    /// Return whether this terminal can display up to 16 colors.
    auto has_extended_colors() const -> bool;

    /// Return the number of colors this terminal supports.
    auto color_count() const -> short;

    /// Return whether this terminal can change color definitions.
    /** This is required for the color Palette to be changed. Return false if
     *  the Terminal hasn't been initialized yet. */
    auto can_change_colors() const -> bool;

    /// Return the number of color pairs in this terminal.
    /** Always returns 0 ...*/
    auto color_pair_count() const -> int;

    /// Map pairs of colors to a unique index between [0, 255]
    auto color_index(Color fg, Color bg) const -> short;

    /// Returns the number of colors in the currently set ANSI_palette.
    auto get_palette_color_count() const -> Color::Value_t
    {
        return static_cast<Color::Value_t>(palette_.size());
    }

   private:
    Palette palette_;
    std::chrono::milliseconds refresh_rate_{33};
    Dynamic_color_engine dynamic_color_engine_;
    Glyph background_    = L' ';
    bool is_initialized_ = false;
    bool show_cursor_    = false;
    bool raw_mode_       = false;

   private:
    /// Actually set raw/noraw mode via ncurses using the state of raw_mode_.
    void ncurses_set_raw_mode() const;

    /// Actually set show_cursor via ncurses using the state of show_cursor_.
    void ncurses_set_cursor() const;

    /// Repaint All Widgets
    void repaint_all();
};

}  // namespace ox
#endif  // TERMOX_TERMINAL_TERMINAL_HPP
