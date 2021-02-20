#ifndef TERMOX_TERMINAL_TERMINAL_HPP
#define TERMOX_TERMINAL_TERMINAL_HPP
#include <cstdint>
#include <optional>

#include <signals_light/signal.hpp>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/system/event_fwd.hpp>
#include <termox/terminal/detail/screen_buffers.hpp>
#include <termox/terminal/dynamic_color_engine.hpp>
#include <termox/widget/area.hpp>

#include <esc/esc.hpp>

namespace ox {

using Mouse_mode = ::esc::Mouse_mode;
using Signals    = ::esc::Signals;

class Terminal {
   public:
    inline static sl::Signal<void(Palette const&)> palette_changed;

    inline static detail::Screen_buffers screen_buffers{Area{0, 0}};

   public:
    /// Initializes the terminal screen into curses mode.
    /** Must be called before any input/output can occur. No-op if initialized.
     *  Mouse_mode  - - Off:   Generates no Mouse Events.
     *                  Basic: Generate Mouse Press and Release Events for all
     *                         buttons and the scroll wheel.
     *                  Drag:  Basic, plus Mouse Move Events while a button is
     *                         pressed.
     *                  Move:  Basic, plus Mouse Move Events are generated with
     *                         or without a button pressed.
     *
     *  Signals - - - On:  Signals can be generated from ctrl-[key] presses, for
     *                     instance ctrl-c will send SIGINT instead of byte 3.
     *                Off: Signals will not be generated on ctrl-[key] presses,
     *                     sending the byte value of the ctrl character instead.
     */
    static void initialize(Mouse_mode mouse_mode = Mouse_mode::Basic,
                           Signals signals       = Signals::On);

    /// Reset the terminal to its state before initialize() was called.
    /** No-op if already uninitialized. */
    static void uninitialize();

    /// Return the Area of the terminal screen.
    [[nodiscard]] static auto area() -> Area { return ::esc::terminal_area(); }

    /// Update the screen to reflect change made by Painter since last call.
    /** This leaves the cursor in an unknown location, the cursor must be set
     *  separately for the currently in-focus Widget. */
    static void refresh();

    /// Update a Color Palette value.
    /** Used by Dynamic_color_engine. */
    static void update_color_stores(Color c, True_color tc);

    /// Repaints all Glyphs with \p c in their Brush to the screen.
    /** Used by Dynamic_color_engine. */
    static void repaint_color(Color c);

    /// Change Color definitions.
    static void set_palette(Palette colors);

    /// Append a Color_definition::Value_t to the current color palette.
    /** Returns the Color that \p def was paired with. Picks the Color by
     *  incrementing the last color in the current palette. */
    static auto palette_append(Color_definition::Value_t value) -> Color;

    /// Return a copy of the currently set color palette.
    [[nodiscard]] static auto current_palette() -> Palette const&
    {
        return palette_;
    }

    /// Set whether or not the cursor is visible on screen.
    static void show_cursor(bool show = true)
    {
        ::esc::set(show ? ::esc::Cursor::Show : ::esc::Cursor::Hide);
        ::esc::flush();
    }

    /// Moves the cursor to Point \p point on screen.
    /** {0, 0} is top left of the terminal screen. */
    static void move_cursor(Point point)
    {
        ::esc::write(::esc::escape(::esc::Cursor_position{point}));
        ::esc::flush();
    }

    /// Return the number of colors in the terminal's built in palette.
    /** This cooresponds to the number of Color_index values. */
    [[nodiscard]] static auto color_count() -> std::uint16_t
    {
        return ::esc::color_palette_size();
    }

    /// Return true if this terminal supports true color.
    [[nodiscard]] static auto has_true_color() -> bool
    {
        return ::esc::has_true_color();
    }

    /// Wait for user input, and return with a corresponding Event.
    /** Blocking call, input can be received from the keyboard, mouse, or the
     *  terminal being resized. Will return nullopt if there is an error. */
    [[nodiscard]] static auto read_input() -> Event;

    /// Sets a flag so that the next call to refresh() will repaint every cell.
    /** The repaint forces the diff to contain every cell on the terminal. */
    static void flag_full_repaint() { full_repaint_ = true; }

    /// Flushes all of the staged changes to the screen and sets the cursor.
    static void flush_screen();

   private:
    inline static Palette palette_;
    inline static Dynamic_color_engine dynamic_color_engine_;
    inline static bool is_initialized_ = false;
    inline static bool full_repaint_   = false;

    /// Return true if \p point is within \p area.
    [[nodiscard]] static auto is_within(Point point, Area area) -> bool
    {
        return point.x < area.width && point.y < area.height;
    }
};

}  // namespace ox
#endif  // TERMOX_TERMINAL_TERMINAL_HPP
