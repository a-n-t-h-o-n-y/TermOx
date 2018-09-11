#ifndef CPPURSES_SYSTEM_TERMINAL_PROPERTIES_HPP
#define CPPURSES_SYSTEM_TERMINAL_PROPERTIES_HPP
#include <cstddef>

#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/palette.hpp>

namespace cppurses {
namespace detail {
class NCurses_event_listener;
class NCurses_paint_engine;
}  // namespace detail

/// Responsible for getting and setting properties of the terminal screen.
class Terminal_properties {
   public:
    Terminal_properties();

    /// Returns the width of the terminal screen.
    std::size_t width() const;

    /// Returns the height of the terminal screen.
    std::size_t height() const;

    /// Set the Glyph used as background for each Widget, if doesn't have one.
    void set_background_tile(const Glyph& tile);

    /// Retrieve the currently used background tile.
    const Glyph& background_tile() const;

    /// Sets color definitions for the entire terminal.
    void set_color_palette(const Palette& palette);

    /// If set true, terminal will pass control characters to your program.
    /** Otherwise the terminal will interpret these characters and not pass them
     *  on to CPPurses. On intialization this is set false. */
    void handle_control_characters(bool handle);

   private:
    std::size_t width_{0};
    std::size_t height_{0};

    Glyph default_background_{L' '};

    void update_dimensions();
    friend detail::NCurses_event_listener;
    friend detail::NCurses_paint_engine;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_TERMINAL_PROPERTIES_HPP
