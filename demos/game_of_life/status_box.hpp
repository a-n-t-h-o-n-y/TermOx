#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_STATUS_BOX_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_STATUS_BOX_HPP
#include <cstdint>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/align.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/line_edit.hpp>
#include <cppurses/widget/widgets/number_edit.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

#include "make_break.hpp"

namespace gol {

class Generation_count : public cppurses::layout::Horizontal<> {
   public:
    Generation_count()
    {
        this->height_policy.fixed(1);
        this->cursor.disable();
        title_ | cppurses::pipe::fixed_width(11uL);
        count_ | cppurses::pipe::fixed_width(5uL);
    }

    void update_count(std::uint32_t count)
    {
        count_.set_contents(std::to_string(count));
    }

   private:
    cppurses::HLabel& title_ =
        this->make_child<cppurses::HLabel>({L"Generation"});

    cppurses::Text_display& count_ =
        this->make_child<cppurses::Text_display>(L"0");
};

struct Center_offset : cppurses::layout::Vertical<> {
   public:
    cppurses::HLabel& title_ = this->make_child<cppurses::HLabel>(
        {L"Center Offset" | cppurses::Trait::Underline});

    cppurses::Labeled_number_edit<>& x_coords =
        this->make_child<cppurses::Labeled_number_edit<>>(L"x: ", 0);

    cppurses::Labeled_number_edit<>& y_coords =
        this->make_child<cppurses::Labeled_number_edit<>>(L"y: ", 0);

   public:
    Center_offset()
    {
        using namespace cppurses;
        title_.set_alignment(Align::Center);
        x_coords.number_edit.brush.set_background(color::Black);
        x_coords.number_edit.brush.set_foreground(color::White);
        x_coords.number_edit.set_ghost_color(color::White);
        y_coords.number_edit.brush.set_background(color::Black);
        y_coords.number_edit.brush.set_foreground(color::White);
        y_coords.number_edit.set_ghost_color(color::White);
    }
};

struct Status_box : cppurses::layout::Vertical<> {
   public:
    Generation_count& gen_count  = this->make_child<Generation_count>();
    Widget& break_               = this->append_child(make_break());
    Center_offset& center_offset = this->make_child<Center_offset>();

   public:
    Status_box() { this->height_policy.fixed(5uL); }
};
}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_STATUS_BOX_HPP
