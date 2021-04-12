#ifndef TERMOX_DEMOS_GAME_OF_LIFE_STATUS_BOX_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_STATUS_BOX_HPP
#include <cstdint>

#include <termox/painter/glyph_string.hpp>
#include <termox/widget/align.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/widgets/label.hpp>
#include <termox/widget/widgets/number_edit.hpp>
#include <termox/widget/widgets/text_display.hpp>
#include <termox/widget/widgets/textline.hpp>

#include "make_break.hpp"

namespace gol {

class Generation_count : public ox::layout::Horizontal<> {
   public:
    Generation_count()
    {
        this->height_policy.fixed(1);
        this->cursor.disable();
        title_ | ox::pipe::fixed_width(11);
        count_ | ox::pipe::fixed_width(5);
    }

    void update_count(std::uint32_t count)
    {
        count_.set_contents(std::to_string(count));
    }

   private:
    ox::HLabel& title_ = this->make_child<ox::HLabel>({U"Generation"});

    ox::Text_display& count_ = this->make_child<ox::Text_display>(U"0");
};

struct Center_offset : ox::layout::Vertical<> {
   public:
    ox::HLabel& title_ =
        this->make_child<ox::HLabel>({U"Center Offset" | ox::Trait::Underline});

    ox::Labeled_number_edit<>& x_coords =
        this->make_child<ox::Labeled_number_edit<>>(U"x: ", 0);

    ox::Labeled_number_edit<>& y_coords =
        this->make_child<ox::Labeled_number_edit<>>(U"y: ", 0);

   public:
    Center_offset()
    {
        using namespace ox;

        title_.set_alignment(Align::Center);

        x_coords.number_edit.brush.background = color::Black;
        x_coords.number_edit.brush.foreground = color::White;
        x_coords.number_edit.set_ghost_color(color::White);

        y_coords.number_edit.brush.background = color::Black;
        y_coords.number_edit.brush.foreground = color::White;
        y_coords.number_edit.set_ghost_color(color::White);
    }
};

struct Status_box : ox::layout::Vertical<> {
   public:
    Generation_count& gen_count  = this->make_child<Generation_count>();
    Widget& break_               = this->append_child(make_break());
    Center_offset& center_offset = this->make_child<Center_offset>();

   public:
    Status_box() { this->height_policy.fixed(5); }
};
}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_STATUS_BOX_HPP
