#ifndef CPPURSES_TERMINAL_TERMINAL_HPP
#define CPPURSES_TERMINAL_TERMINAL_HPP
#include <cstddef>

#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/palette.hpp>
#include <cppurses/painter/palettes.hpp>

namespace cppurses {

class Terminal {
   public:
    /// Initializes the terminal screen into curses mode.
    /** Must be called before any input/output can occur. Also initializes
     *  various proerties that are modifiable from this Terminal class. No-op if
     *  already initialized. */
    void initialize();

    /// Resets the terminal to its state before initialize() was called.
    /** No-op if already uninitialized. */
    void uninitialize();

    /// Returns the width of the terminal screen.
    std::size_t width() const;

    /// Returns the height of the terminal screen.
    std::size_t height() const;

    /// Resizes the terminal's internal data structures to be of the given size.
    /** Does not actually resize your terminal's window. */
    void resize(std::size_t width, std::size_t height);

    /// Sets the default background/wallpaper tiles to be used.
    /** This is used if a Widget has no assigned wallpaper. */
    void set_background(const Glyph& tile);

    /// Returns the default background/wallpaper currently in use.
    const Glyph& background() const { return background_; }

    /// Sets terminal color definitions for the 16 Colors in CPPurses.
    void set_color_palette(const Palette& palette);

    /// Sets whether or not the cursor is visible on screen.
    void show_cursor(bool show = true);

    /// Enables or disables raw mode.
    /** In raw mode, the interrupt, quit, suspend, and flow control characters
     *  are all passed through uninterpreted, instead of generating a signal.
     *  This mode is off by default. */
    void raw_mode(bool enable = true);

    /// Returns whether this terminal can display colors.
    /** Returns false if the Terminal hasn't been initialized yet. */
    bool has_color() const;

    /// Returns whether this terminal can change color definitions.
    /** This is required for the color Palette to be changed. Returns false if
     *  the Terminal hasn't been initialized yet. */
    bool can_change_colors() const;

   private:
    bool is_initialized_{false};
    bool show_cursor_{false};
    Glyph background_{L' '};
    Palette palette_{Palettes::DawnBringer()};
    bool raw_mode_{false};

    /// Registers the input::indicate_resize signal handler for sigwinch signal.
    void setup_resize_signal_handler() const;

    /// Actually sets the palette via ncurses using the state of palette_.
    void ncurses_set_palette() const;

    /// Actually sets raw/noraw mode via ncurses using the state of raw_mode_.
    void ncurses_set_raw_mode() const;

    /// Actually sets show_cursor via ncurses using the state of show_cursor_.
    void ncurses_set_cursor() const;
};

}  // namespace cppurses
#endif  // CPPURSES_TERMINAL_TERMINAL_HPP
