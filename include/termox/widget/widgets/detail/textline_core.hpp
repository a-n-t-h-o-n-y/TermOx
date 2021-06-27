#ifndef TERMOX_WIDGET_WIDGETS_DETAIL_TEXTLINE_CORE_HPP
#define TERMOX_WIDGET_WIDGETS_DETAIL_TEXTLINE_CORE_HPP
#include <termox/painter/glyph.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/widget/align.hpp>

namespace ox::detail {

/// Provides a view of a Glyph_string and cursor position over a fixed length.
class Textline_core {
   public:
    Textline_core(ox::Glyph_string initial,
                  int display_length,
                  ox::Align alignment,
                  int cursor_index = 0);

   public:
    /// Clear and reset the text.
    void set_text(ox::Glyph_string x);

    /// Return the full text contents.
    [[nodiscard]] auto text() const noexcept -> ox::Glyph_string const&;

    /// Erase all text and reset anchor and cursor indices.
    void clear();

    /// Set the number of cells that the text will be displayed in.
    void set_display_length(int x);

    /// Return the current display_length.
    [[nodiscard]] auto display_length() const noexcept -> int;

    /// Set either Left or Right Alignment, display_length interacts with this.
    void set_alignment(ox::Align x);

    /// Return the current alignment.
    [[nodiscard]] auto alignment() const noexcept -> ox::Align;

    /// Move the cursor one position to the right, scrolling the text if needed.
    void increment_cursor();

    /// Move the cursor one position to the left, scrolling the text if needed.
    void decrement_cursor();

    /// Move the cursor to the closest index to display position \p x.
    void move_cursor_to_display(int x);

    /// Move the cursor to the Glyph at index \p i into the text, scrolls.
    void set_cursor_to_index(int i);

    /// Move the Cursor to the first index and scroll.
    void cursor_home();

    /// Move the Cursor to the last index and scroll.
    void cursor_end();

    /// Inserts \p x at the current cursor index.
    void insert_at_cursor(ox::Glyph x);

    /// Remove the Glyph at the cursor index.
    void erase_at_cursor();

    /// Remove the Glyph at one less the cursor index, if valid.
    void erase_before_cursor();

    /// Return the position along the display length that the cursor is at.
    [[nodiscard]] auto cursor_position() const noexcept -> int;

    /// Return a substring that fits within width, accounting for cursor scroll.
    /** Appends a trailing space if the last char plus one space is open. */
    [[nodiscard]] auto display_substr() const -> ox::Glyph_string;

   private:
    ox::Glyph_string text_;
    int display_length_;
    ox::Align alignment_;
    int cursor_index_;
    int anchor_index_ = 0;  // The left most displayable index.
};

}  // namespace ox::detail
#endif  // TERMOX_WIDGET_WIDGETS_DETAIL_TEXTLINE_CORE_HPP
