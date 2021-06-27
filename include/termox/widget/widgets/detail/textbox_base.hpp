#ifndef TERMOX_WIDGET_WIDGETS_DETAIL_TEXTBOX_BASE_HPP
#define TERMOX_WIDGET_WIDGETS_DETAIL_TEXTBOX_BASE_HPP
#include <cstddef>

#include <signals_light/signal.hpp>

#include <termox/painter/glyph_string.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widgets/text_view.hpp>

namespace ox::detail {

/// Implements cursor movement on top of a Text_view, for use by Textbox.
class Textbox_base : public Text_view {
   private:
    using Text_view::Parameters;

   protected:
    /// Construct with initial \p text, enable cursor.
    explicit Textbox_base(Glyph_string text  = U"",
                          Align alignment    = Align::Left,
                          Wrap wrap          = Wrap::Word,
                          Brush insert_brush = Brush{});

    explicit Textbox_base(Parameters p);

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
    void set_cursor(Point position);

    /// Set the cursor to the Glyph at \p index into contents.
    void set_cursor(std::size_t index);

    /// Add cursor movement to Text_view::scroll_up.
    void scroll_up(int n = 1) override;

    /// Add cursor movement to Text_view::scroll_down.
    void scroll_down(int n = 1) override;

    /// Move the cursor up \p n lines, scroll if at the top line.
    /** Cursor is moved to the right-most Glyph if the moved to line does not
     *  extend as far as the current cursor column position. */
    void cursor_up(int n = 1);

    /// Move the cursor down \p n lines, scroll if at the bottom line.
    /** Cursor is moved to the right-most Glyph if the moved to line does not
     *  extend as far as the current cursor column position. */
    void cursor_down(int n = 1);

    /// Move the cursor \p n indices towards the beginning of contents.
    /** Scroll up if moving past the top-left position. */
    void cursor_left(int n = 1);

    /// Move the cursor \p n indices towards the end of contents.
    /** Scroll down if moving past the bottom-right position. */
    void cursor_right(int n = 1);

   protected:
    /// Return the index into contents that the cursor is located at.
    [[nodiscard]] auto cursor_index() const -> int;

    /// Scroll to make the cursor visible if no longer on screen after resize.
    auto resize_event(Area new_size, Area old_size) -> bool override;

   private:
    /// Move the cursor one position to the right.
    void increment_cursor_right();

    /// Move the cursor one position to the left.
    void increment_cursor_left();
};

}  // namespace ox::detail
#endif  // TERMOX_WIDGET_WIDGETS_DETAIL_TEXTBOX_BASE_HPP
