#include "status_box.hpp"

#include <cctype>
#include <cstdint>
#include <string>
#include <utility>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/line_edit.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

using namespace cppurses;
namespace gol {

Generation_count::Generation_count() {
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(1);
    this->cursor.disable();
}

void Generation_count::update_count(std::uint32_t count) {
    count_.set_contents(std::to_string(count));
}

Center_offset::Center_offset() {
    title_.set_alignment(Alignment::Center);
    x_coords.number_edit.brush.set_background(cppurses::Color::Black);
    x_coords.number_edit.brush.set_foreground(cppurses::Color::White);
    x_coords.number_edit.set_ghost_color(Color::White);
    y_coords.number_edit.brush.set_background(cppurses::Color::Black);
    y_coords.number_edit.brush.set_foreground(cppurses::Color::White);
    y_coords.number_edit.set_ghost_color(Color::White);
}

Status_box::Status_box() {
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(5);

    this->border.enable();
    this->border.segments.disable_all();
    this->border.segments.north.enable();
    this->border.segments.north = Glyph{L'─', foreground(Color::Blue)};
}
}  // namespace gol
