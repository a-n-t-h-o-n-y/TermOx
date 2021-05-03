#ifndef TERMOX_WIDGET_WIDGETS_DETAIL_TEXTLINE_CORE_HPP
#define TERMOX_WIDGET_WIDGETS_DETAIL_TEXTLINE_CORE_HPP
#include <algorithm>
#include <cassert>
#include <iterator>

#include <iostream>  //temp

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
                  int cursor_index = 0)
        : text_{std::move(initial)},
          display_length_{display_length},
          alignment_{alignment},
          cursor_index_{cursor_index}
    {}

   public:
    /// Clear and reset the text.
    void set_text(ox::Glyph_string x)
    {
        text_ = std::move(x);
        if (cursor_index_ > text_.size()) {
            cursor_index_ = 0;
            anchor_index_ = 0;
        }
    }

    /// Return the full text contents.
    [[nodiscard]] auto text() const noexcept -> ox::Glyph_string const&
    {
        return text_;
    }

    /// Erase all text and reset anchor and cursor indices.
    void clear()
    {
        text_         = U"";
        cursor_index_ = 0;
        anchor_index_ = 0;
    }

    /// Set the number of cells that the text will be displayed in.
    void set_display_length(int x)
    {
        display_length_ = x;
        if (alignment_ == ox::Align::Right &&
            ((text_.size() - anchor_index_) < display_length_) &&
            anchor_index_ != 0) {
            anchor_index_ = std::max(text_.size() - (display_length_ - 1), 0);
        }
        auto const last_display_index =
            anchor_index_ + std::max(display_length_ - 1, 0);
        if (cursor_index_ > last_display_index)
            cursor_index_ = last_display_index;
    }

    /// Return the current display_length.
    [[nodiscard]] auto display_length() const noexcept -> int
    {
        return display_length_;
    }

    /// Set either Left or Right Alignment, display_length interacts with this.
    void set_alignment(ox::Align x)
    {
        assert(x == ox::Align::Left || x == ox::Align::Right);
        alignment_ = x;
        this->set_display_length(display_length_);  // reset anchor_index_
    }

    /// Return the current alignment.
    [[nodiscard]] auto alignment() const noexcept -> ox::Align
    {
        return alignment_;
    }

    /// Move the cursor one position to the right, scrolling the text if needed.
    void increment_cursor()
    {
        // Cursor index can be == text_.size()
        if (cursor_index_ < text_.size())
            ++cursor_index_;
        if (cursor_index_ - anchor_index_ >= display_length_)
            ++anchor_index_;
    }

    /// Move the cursor one position to the left, scrolling the text if needed.
    void decrement_cursor()
    {
        if (cursor_index_ != 0)
            --cursor_index_;
        if (cursor_index_ < anchor_index_)
            --anchor_index_;
    }

    /// Move the cursor to the closest index to display position \p x.
    void move_cursor_to_display(int x)
    {
        if (alignment_ == ox::Align::Right) {
            x -= std::max(display_length_ - 1 - (text_.size() - anchor_index_),
                          0);
            x = std::max(x, 0);
        }
        cursor_index_ = std::min(anchor_index_ + x, text_.size());
    }

    /// Move the cursor to the Glyph at index \p i into the text, scrolls.
    void set_cursor_to_index(int i)
    {
        cursor_index_ = i;
        if (cursor_index_ < anchor_index_)
            anchor_index_ = cursor_index_;
        else if (display_length_ != 0 &&
                 cursor_index_ - anchor_index_ > display_length_) {
            anchor_index_ = cursor_index_ - display_length_;
        }
    }

    /// Move the Cursor to the first index and scroll.
    void cursor_home()
    {
        cursor_index_ = 0;
        anchor_index_ = 0;
    }

    /// Move the Cursor to the last index and scroll.
    void cursor_end()
    {
        cursor_index_ = text_.size();
        if (cursor_index_ - anchor_index_ > display_length_)
            anchor_index_ = std::max(text_.size() - display_length_ + 1, 0);
    }

    /// Inserts \p x at the current cursor index.
    void insert_at_cursor(ox::Glyph x)
    {
        auto const at = std::next(std::begin(text_), cursor_index_);
        text_.insert(at, x);
        this->increment_cursor();
    }

    /// Remove the Glyph at the cursor index.
    void erase_at_cursor()
    {
        if (cursor_index_ == text_.size())
            return;
        auto const at = std::next(std::begin(text_), cursor_index_);
        text_.erase(at);
        if (alignment_ == ox::Align::Right)
            anchor_index_ = std::max(anchor_index_ - 1, 0);
        if (cursor_index_ > text_.size())
            this->decrement_cursor();
    }

    /// Remove the Glyph at one less the cursor index, if valid.
    void erase_before_cursor()
    {
        if (cursor_index_ == 0)
            return;
        auto const at = std::next(std::begin(text_), cursor_index_ - 1);
        text_.erase(at);
        if (alignment_ == ox::Align::Right)
            anchor_index_ = std::max(anchor_index_ - 1, 0);
        this->decrement_cursor();
    }

    /// Return the position along the display length that the cursor is at.
    [[nodiscard]] auto cursor_position() const noexcept -> int
    {
        return cursor_index_ - anchor_index_;
    }

    /// Return a substring that fits within width, accounting for cursor scroll.
    /** Appends a trailing space if the last char plus one space is open. */
    [[nodiscard]] auto display_substr() const -> ox::Glyph_string
    {
        auto const first = std::next(std::begin(text_), anchor_index_);
        auto const last  = [&] {
            auto const end_index =
                std::min(anchor_index_ + display_length_, text_.size());
            return std::next(std::begin(text_), end_index);
        }();
        auto result = ox::Glyph_string{first, last};
        if (result.size() < display_length_)
            result.push_back(U' ');  // For Cursor
        return result;
    }

   private:
    ox::Glyph_string text_;
    int display_length_;
    ox::Align alignment_;
    int cursor_index_;
    int anchor_index_ = 0;  // The left most displayable index.
};

}  // namespace ox::detail
#endif  // TERMOX_WIDGET_WIDGETS_DETAIL_TEXTLINE_CORE_HPP
