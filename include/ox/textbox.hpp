#pragma once

#include <utility>
#include <vector>

#include <ox/core/core.hpp>
#include <ox/widget.hpp>

namespace ox {

/**
 * A box of text.
 *
 * @details Set FocusPolicy::None to disable editing.
 */
class TextBox : public Widget {
   public:
    std::vector<Glyph> text;
    enum class Wrap { Any, Word } wrap;
    enum class Align { Left, Center, Right } align;
    Color background;
    Brush insert_brush;

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
    int top_line_ = 0;
    std::size_t cursor_index_ = 0;  // Can be one past the end of text.
};

}  // namespace ox