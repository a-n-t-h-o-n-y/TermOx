#ifndef PAINTER_DETAIL_NCURSES_PAINT_ENGINE_HPP
#define PAINTER_DETAIL_NCURSES_PAINT_ENGINE_HPP
#include <cstddef>
#include <cstdint>

#include <cppurses/painter/color.hpp>

/* Paint_engine Requires:
 *   void put_glyph(const Glyph& g);
 *   void put(std::size_t x, std::size_t y, const Glyph& g) {
 *   void move_cursor(std::size_t x, std::size_t y);
 *   void show_cursor(bool show = true);
 *   void hide_cursor(bool hide = true);
 *   std::size_t screen_width();
 *   std::size_t screen_height();
 *   void refresh();
 *   void set_rgb(Color c, std::int16_t r, std::int16_t g, std::int16_t b);
 */

namespace cppurses {
struct Glyph;
namespace detail {

/// Provides access to basic painting functionality, using ncurses internally.
class NCurses_paint_engine {
   public:
    /// Initializes the terminal screen for painting.
    NCurses_paint_engine();

    /// Restores the terminal screen to the original state, before program run.
    ~NCurses_paint_engine();

    NCurses_paint_engine(const NCurses_paint_engine&) = delete;
    NCurses_paint_engine(NCurses_paint_engine&&) = default;
    NCurses_paint_engine& operator=(const NCurses_paint_engine&) = delete;
    NCurses_paint_engine& operator=(NCurses_paint_engine&&) = default;

    /// Places a Glyph \p g on the screen at the current cursor position.
    void put_glyph(const Glyph& g);

    /// Places a Glyph \p g at coordinates \p x , \p y. (0,0) is top left.
    void put(std::size_t x, std::size_t y, const Glyph& g);

    /// Moves the cursor to point \p x , \p y on screen. (0,0) is top left.
    void move_cursor(std::size_t x, std::size_t y);

    /// Turns the cursor display on.
    void show_cursor(bool show = true);

    /// Turns the cursor display off.
    void hide_cursor(bool hide = true);

    /// Retrieves the terminal screen width.
    std::size_t screen_width();

    /// Retrieves the terminal screen height.
    std::size_t screen_height();

    /// Flushes all of the changes made by this class to the screen.
    void refresh();

    /// Register a color definition, using values [0-256) for rgb values.
    void set_rgb(Color c, std::int16_t r, std::int16_t g, std::int16_t b);

   private:
    /// Registers a handlers for the window resize signal from the kernal.
    void setup_sigwinch();
};

}  // namespace detail
}  // namespace cppurses
#endif  // PAINTER_DETAIL_NCURSES_PAINT_ENGINE_HPP
