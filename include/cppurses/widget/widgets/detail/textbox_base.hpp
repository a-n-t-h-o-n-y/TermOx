#ifndef CPPURSES_WIDGET_WIDGETS_DETAIL_TEXTBOX_BASE_HPP
#define CPPURSES_WIDGET_WIDGETS_DETAIL_TEXTBOX_BASE_HPP
#include <cstddef>

#include <signals/signal.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

namespace cppurses {
struct Point;
namespace detail {

/// Implements cursor movement on top of a Text_display, for use by Textbox.
class Textbox_base : public Text_display {
   public:
    /// Set the cursor to \p position, or the nearest Glyph if no glyph there.
    void set_cursor(Point position) {
        this->set_cursor(this->index_at(position));
    }

    /// Set the cursor to the Glyph at \p index into contents.
    void set_cursor(std::size_t index);

    /// Add cursor movement to Text_display::scroll_up.
    void scroll_up(std::size_t n = 1) override;

    /// Add cursor movement to Text_display::scroll_down.
    void scroll_down(std::size_t n = 1) override;

    /// Move the cursor up \p n lines, scroll if at the top line.
    /** Cursor is moved to the right-most Glyph if the moved to line does not
     *  extend as far as the current cursor column position. */
    void cursor_up(std::size_t n = 1);

    /// Move the cursor down \p n lines, scroll if at the bottom line.
    /** Cursor is moved to the right-most Glyph if the moved to line does not
     *  extend as far as the current cursor column position. */
    void cursor_down(std::size_t n = 1);

    /// Move the cursor \p n indices towards the beginning of contents.
    /** Scroll up if moving past the top-left position. */
    void cursor_left(std::size_t n = 1);

    /// Move the cursor \p n indices towards the end of contents.
    /** Scroll down if moving past the bottom-right position. */
    void cursor_right(std::size_t n = 1);

    /// Emitted when the cursor moves left, passes along \n positions moved.
    sig::Signal<void(std::size_t n)> cursor_moved_left;

    /// Emitted when the cursor moves right, passes along \n positions moved.
    sig::Signal<void(std::size_t n)> cursor_moved_right;

    /// Emitted when the cursor moves up, passes along \n positions moved.
    sig::Signal<void(std::size_t n)> cursor_moved_up;

    /// Emitted when the cursor moves down, passes along \n positions moved.
    sig::Signal<void(std::size_t n)> cursor_moved_down;

   protected:
    /// Construct with initial \p contents, enable cursor.
    explicit Textbox_base(Glyph_string contents = "");

    /// Return the index into contents that the cursor is located at.
    std::size_t cursor_index() const {
        return this->index_at(this->cursor.position());
    }

    /// Scroll to make the cursor visible if no longer on screen after resize.
    bool resize_event(Area new_size, Area old_size) override;

   private:
    /// Move the cursor one position to the right.
    void increment_cursor_right();

    /// Move the cursor one position to the left.
    void increment_cursor_left();
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_DETAIL_TEXTBOX_BASE_HPP
