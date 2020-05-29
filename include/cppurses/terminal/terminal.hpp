#ifndef CPPURSES_TERMINAL_TERMINAL_HPP
#define CPPURSES_TERMINAL_TERMINAL_HPP
#include <chrono>
#include <cstddef>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/rgb.hpp>

namespace cppurses {

class Terminal {
   public:
    /// Initializes the terminal screen into curses mode.
    /** Must be called before any input/output can occur. Also initializes
     *  various proerties that are modifiable from this Terminal class. No-op if
     *  already initialized. */
    void initialize();

    /// Reset the terminal to its state before initialize() was called.
    /** No-op if already uninitialized. */
    void uninitialize();

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
    void set_background(Glyph const& tile);

    /// Return the default background/wallpaper currently in use.
    auto background() const -> Glyph const& { return background_; }

    /// Set ANSI terminal color definitions for CPPurses Colors.
    void set_palette(ANSI_palette const& colors);

    /// Set True Color definitions for CPPurses Colors and ANSI values.
    void set_palette(True_color_palette const& colors);

    /// Return a copy of the currently set color palette.
    auto current_palette() const -> ANSI_palette const& { return palette_; }

    /// Set the RGB value of an ANSI color.
    void set_color_definition(True_color_definition const& def);

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
    auto color_pair_count() const -> short;

    /// Map pairs of colors to a unique index between [0, 255]
    auto color_index(Color fg, Color bg) const -> short;

    /// Returns the number of colors in the currently set ANSI_palette.
    auto get_ansi_color_count() const -> Color::Value_t
    {
        return static_cast<Color::Value_t>(palette_.size());
    }

    /// Returns the number of colors in the currently set True_color_palette.
    auto get_true_color_count() const -> Color::Value_t
    {
        return static_cast<Color::Value_t>(tc_palette_.size());
    }

   private:
    bool is_initialized_ = false;
    bool show_cursor_    = false;
    bool raw_mode_       = false;
    Glyph background_    = L' ';
    ANSI_palette palette_;
    True_color_palette tc_palette_;
    std::chrono::milliseconds refresh_rate_{33};

   private:
    /// Actually set raw/noraw mode via ncurses using the state of raw_mode_.
    void ncurses_set_raw_mode() const;

    /// Actually set show_cursor via ncurses using the state of show_cursor_.
    void ncurses_set_cursor() const;

    auto get_ansi_value(Color c) -> ANSI::Value_t;

    /// Repaint All Widgets
    void repaint_all();
};

}  // namespace cppurses
#endif  // CPPURSES_TERMINAL_TERMINAL_HPP
