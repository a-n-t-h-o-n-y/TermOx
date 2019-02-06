#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_STATUS_BOX_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_STATUS_BOX_HPP
#include <cstdint>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal_layout.hpp>
#include <cppurses/widget/layouts/vertical_layout.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/line_edit.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

namespace gol {

class Generation_count : public cppurses::Horizontal_layout {
   public:
    Generation_count();

    void update_count(std::uint32_t count);

   private:
    cppurses::Label& title_{this->make_child<cppurses::Label>("Gen #: ")};
    cppurses::Text_display& count_{
        this->make_child<cppurses::Text_display>("0")};
};

struct Labeled_line_edit : cppurses::Horizontal_layout {
    Labeled_line_edit(cppurses::Glyph_string label_text,
                      cppurses::Glyph_string initial);

    cppurses::Label& label;
    cppurses::Line_edit& value_display;
};

struct Center_offset : cppurses::Vertical_layout {
    Center_offset();

    cppurses::Label& title_{
        this->make_child<cppurses::Label>(cppurses::Glyph_string{
            "Center Offset", cppurses::Attribute::Underline})};
    Labeled_line_edit& x_coords{
        this->make_child<Labeled_line_edit>("x: ", "0")};
    Labeled_line_edit& y_coords{
        this->make_child<Labeled_line_edit>("y: ", "0")};
};

struct Status_box : cppurses::Vertical_layout {
    Status_box();

    Generation_count& gen_count{this->make_child<Generation_count>()};
    Center_offset& center_offset{this->make_child<Center_offset>()};
};
}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_STATUS_BOX_HPP
