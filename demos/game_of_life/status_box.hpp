#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_STATUS_BOX_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_STATUS_BOX_HPP
#include <cstdint>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/line_edit.hpp>
#include <cppurses/widget/widgets/number_edit.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

namespace gol {

class Generation_count : public cppurses::layout::Horizontal<> {
   public:
    Generation_count()
    {
        this->height_policy.fixed(1);
        this->cursor.disable();
    }

    void update_count(std::uint32_t count)
    {
        count_.set_contents(std::to_string(count));
    }

   private:
    cppurses::Label& title_ = this->make_child<cppurses::Label>("Gen #");
    cppurses::Text_display& count_ =
        this->make_child<cppurses::Text_display>("0");
};

struct Center_offset : cppurses::layout::Vertical<> {
   public:
    cppurses::Label& title_ = this->make_child<cppurses::Label>(
        cppurses::Glyph_string{"Center Offset", cppurses::Trait::Underline});
    cppurses::Labeled_number_edit<>& x_coords =
        this->make_child<cppurses::Labeled_number_edit<>>("x: ", 0);
    cppurses::Labeled_number_edit<>& y_coords =
        this->make_child<cppurses::Labeled_number_edit<>>("y: ", 0);

   public:
    Center_offset();
};

struct Status_box : cppurses::layout::Vertical<> {
   public:
    Generation_count& gen_count  = this->make_child<Generation_count>();
    Center_offset& center_offset = this->make_child<Center_offset>();

   public:
    Status_box();
};
}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_STATUS_BOX_HPP
