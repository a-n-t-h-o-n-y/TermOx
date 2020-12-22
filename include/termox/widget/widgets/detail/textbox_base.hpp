#ifndef TERMOX_WIDGET_WIDGETS_DETAIL_TEXTBOX_BASE_HPP
#define TERMOX_WIDGET_WIDGETS_DETAIL_TEXTBOX_BASE_HPP
#include <cstddef>
#include <utility>

#include <signals_light/signal.hpp>

#include <termox/painter/glyph_string.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widgets/text_display.hpp>

namespace ox::detail {

/// Implements cursor movement on top of a Text_display, for use by Textbox.
class Textbox_base : public Text_display {
   public:
    /// Emitted when the cursor moves left, passes along \n positions moved.
    sl::Signal<void(std::size_t n)> cursor_moved_left;

    /// Emitted when the cursor moves right, passes along \n positions moved.
    sl::Signal<void(std::size_t n)> cursor_moved_right;

    /// Emitted when the cursor moves up, passes along \n positions moved.
    sl::Signal<void(std::size_t n)> cursor_moved_up;

    /// Emitted when the cursor moves down, passes along \n positions moved.
    sl::Signal<void(std::size_t n)> cursor_moved_down;

   public:
    /// Set the cursor to \p position, or the nearest Glyph if no glyph there.
    void set_cursor(Point position)
    {
        this->set_cursor(this->index_at(position));
    }

    /// Set the cursor to the Glyph at \p index into contents.
    void set_cursor(std::size_t index)
    {
        auto const coords = this->display_position(index);
        this->cursor.set_position(coords);
    }

    /// Add cursor movement to Text_display::scroll_up.
    void scroll_up(std::size_t n = 1) override
    {
        if (this->top_line() == 0)
            return;
        Text_display::scroll_up(n);
        auto y = this->cursor.y() + n;
        if (y > this->height() - 1)
            y = this->height() - 1;
        this->set_cursor({this->cursor.x(), y});
    }

    /// Add cursor movement to Text_display::scroll_down.
    void scroll_down(std::size_t n = 1) override
    {
        Text_display::scroll_down(n);
        auto y = this->cursor.y();
        if (y < n)
            y = 0;
        else
            y -= n;
        this->set_cursor({this->cursor.x(), y});
    }

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
    void cursor_left(std::size_t n = 1)
    {
        for (auto i = 0uL; i < n; ++i) {
            this->increment_cursor_left();
        }
        cursor_moved_left(n);
    }

    /// Move the cursor \p n indices towards the end of contents.
    /** Scroll down if moving past the bottom-right position. */
    void cursor_right(std::size_t n = 1)
    {
        for (auto i = 0uL; i < n; ++i) {
            this->increment_cursor_right();
        }
        cursor_moved_right(n);
    }

   protected:
    /// Construct with initial \p contents, enable cursor.
    explicit Textbox_base(Glyph_string contents = "")
        : Text_display{std::move(contents)}
    {
        this->cursor.enable();
    }

    /// Return the index into contents that the cursor is located at.
    auto cursor_index() const -> std::size_t
    {
        return this->index_at(this->cursor.position());
    }

    /// Scroll to make the cursor visible if no longer on screen after resize.
    auto resize_event(Area new_size, Area old_size) -> bool override;

   private:
    /// Move the cursor one position to the right.
    void increment_cursor_right()
    {
        if (this->cursor_index() == this->contents().size())
            return;
        auto const true_last_index =
            this->first_index_at(this->bottom_line() + 1) - 1;
        auto const cursor_index = this->cursor_index();
        if (cursor_index == true_last_index and
            this->cursor.y() == this->height() - 1) {
            this->scroll_down(1);
        }
        this->set_cursor(cursor_index + 1);
    }

    /// Move the cursor one position to the left.
    void increment_cursor_left()
    {
        auto next_index = this->cursor_index();
        if (this->cursor.position() == Point{0, 0})
            this->scroll_up(1);
        if (next_index == 0)
            return;
        --next_index;
        this->set_cursor(next_index);
    }
};

}  // namespace ox::detail
#endif  // TERMOX_WIDGET_WIDGETS_DETAIL_TEXTBOX_BASE_HPP
