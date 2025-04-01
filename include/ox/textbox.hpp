#pragma once

#include <span>
#include <utility>
#include <vector>

#include <ox/align.hpp>
#include <ox/core/core.hpp>
#include <ox/widget.hpp>

namespace ox {
class ScrollBar;

/**
 * A box of text.
 * @details Set FocusPolicy::None to disable editing.
 */
class TextBox : public Widget {
   public:
    enum class Wrap { Any, Word };

    struct Options {
        std::string text = {};  // UTF8 encoded
        Wrap wrap = Wrap::Word;
        Align align = Align::Left;
        Brush brush = {};
        FocusPolicy focus_policy = FocusPolicy::None;
        SizePolicy size_policy = SizePolicy::flex();
    } static const init;

   public:
    std::string text;
    Wrap wrap;
    Align align;
    Brush brush;

    /**
     * Emitted when any scroll parameter is updated. For ScrollBar.
     * void(int index, int size)
     */
    sl::Signal<void(int, int)> on_scroll;

    TextBox(Options x = init);

   public:
    /**
     * Set the line number for the top display line.
     */
    void set_scroll_offset(int position);

    /**
     * Return the number of lines that can be scrolled to.
     */
    [[nodiscard]]
    auto get_scroll_length() const -> int;

   public:
    void paint(Canvas c) override;

    void key_press(Key k) override;

    void mouse_press(Mouse m) override;

    void mouse_wheel(Mouse m) override;

   private:
    void update_layout_cache();

   private:
    int scroll_offset_ = 0;  // Line number of the top displayed line. zero indexed.
    std::size_t cursor_index_ = 0;                  // [0, unicode_str_.size()]
    std::u32string unicode_str_ = {};               // cache; updated by paint()
    std::vector<std::u32string_view> text_layout_;  // cache; updated by paint()
};

/**
 * Link a ScrollBar to a TextBox, the ScrollBar will control the TextBox, and the
 * TextBox will update the ScrollBar.
 */
void link(TextBox& tb, ScrollBar& sb);

}  // namespace ox