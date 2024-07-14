#pragma once

#include <utility>
#include <vector>

#include <ox/core/core.hpp>
#include <ox/widget.hpp>

namespace ox {
class ScrollBar;

/**
 * A box of text.
 *
 * @details Set FocusPolicy::None to disable editing.
 */
class TextBox : public Widget {
   public:
    /**
     * Emitted when any scroll parameter is updated. For ScrollBar.
     * void(int position, int line_count)
     */
    sl::Signal<void(int, int)> on_scroll_update;

   public:
    std::vector<Glyph> text;
    enum class Wrap { Any, Word } wrap;
    enum class Align { Left, Center, Right } align;
    Color background;
    Brush insert_brush;
    int top_line = 0;

   public:
    TextBox(std::vector<Glyph> text_ = {},
            Wrap wrap_ = Wrap::Word,
            Align align_ = Align::Left,
            Color background_ = XColor::Default,
            Brush insert_brush_ = {});

   public:
    void paint(Canvas c) override;

    void key_press(Key k) override;

    void mouse_press(Mouse m) override;

    void mouse_wheel(Mouse m) override;

   private:
    int line_count_ = 0;
    std::size_t cursor_index_ = 0;  // Can be one past the end of text.
};

/**
 * Link a ScrollBar to a TextBox, the ScrollBar will control the TextBox, and the
 * TextBox will update the ScrollBar.
 */
void link(TextBox& tb, ScrollBar& sb);

}  // namespace ox