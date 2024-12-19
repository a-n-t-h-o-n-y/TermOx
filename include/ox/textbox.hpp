#pragma once

#include <span>
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
    enum class Wrap { Any, Word };
    enum class Align { Left, Center, Right };

    struct Init {
        std::vector<Glyph> text = {};
        Wrap wrap = Wrap::Word;
        Align align = Align::Left;
        Color background = XColor::Default;
        Brush insert_brush = {};
    } static const init;

    TextBox(Init state = init);

   public:
    void set_text(std::vector<Glyph> text);

    [[nodiscard]] auto get_text() const -> std::vector<Glyph> const&;

    void set_wrap(Wrap wrap);

    [[nodiscard]] auto get_wrap() const -> Wrap;

    void set_align(Align align);

    [[nodiscard]] auto get_align() const -> Align;

    void set_background(Color color);

    [[nodiscard]] auto get_background() const -> Color;

    void set_insert_brush(Brush brush);

    [[nodiscard]] auto get_insert_brush() const -> Brush;

    /**
     * Set the line displayed at the top, scrolling the text. Will change the cursor
     * index if the cursor is now off screen.
     */
    void set_top_line(int line);

   public:
    void paint(Canvas c) override;

    void key_press(Key k) override;

    void mouse_press(Mouse m) override;

    void mouse_wheel(Mouse m) override;

    void resize(Area) override;

   private:
    std::vector<Glyph> text_;
    Wrap wrap_;
    Align align_;
    Color background_;
    Brush insert_brush_;

    int top_line_ = 0;
    std::size_t cursor_index_ = 0;  // Can be one past the end of text.
    std::vector<std::span<Glyph const>> spans_ = {};
};

/**
 * Link a ScrollBar to a TextBox, the ScrollBar will control the TextBox, and the
 * TextBox will update the ScrollBar.
 */
void link(TextBox& tb, ScrollBar& sb);

}  // namespace ox