#pragma once

#include <vector>

#include <ox/core/core.hpp>
#include <ox/widget.hpp>

namespace ox {

/**
 * A box of text that displays Glyphs.
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
            Brush insert_brush_ = {})
        : text{std::move(text_)},
          wrap{wrap_},
          align{align_},
          background{background_},
          insert_brush{insert_brush_}
    {}

   public:
    void paint(Canvas c) override;

    void key_press(Key k) override;

    // TODO
    void resize(Area new_size) override
    {
        // TODO is this function override needed?
        // spans are recalculated on paint
        // move cursor here? or in paint?
        // maybe change top_line if cursor is out of view? Or should that be handled in
        // paint as well? This shouldn't scroll
        // There has to be some sort of calculation, because the top line can become
        // invalid, in fact you don't want to hold top line constant here, instead you
        // want to make sure the cursor is still on the screen.
    }

    // TODO
    // moves the cursor
    void mouse_press(Mouse m) override {}

    void mouse_wheel(Mouse m) override
    {
        if (m.button == Mouse::Button::ScrollUp) {
            if (top_line_ > 0) {
                --top_line_;
            }
        }
        else if (m.button == Mouse::Button::ScrollDown) {
            ++top_line_; // TODO if isn't > spans.sise()
        }
    }
    // TODO mouse scroll

   private:
    int top_line_ = 0;
    std::size_t cursor_index_ = 0;  // Can be one past the end of text.
};

}  // namespace ox