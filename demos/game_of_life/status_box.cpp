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

Labeled_line_edit::Labeled_line_edit(Glyph_string label_text,
                                     Glyph_string initial)
    : label{this->make_child<Label>(std::move(label_text))},
      value_display{this->make_child<Line_edit>(std::move(initial))} {
    label.width_policy.type(Size_policy::Fixed);
    label.width_policy.hint(label.contents().size());
    value_display.set_ghost_color(Color::White);
    value_display.set_validator([](char c) { return std::isdigit(c); });
}

Center_offset::Center_offset() {
    title_.set_alignment(Alignment::Center);
}

Status_box::Status_box() {
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(5);
    this->border.enabled = true;
    disable_corners(this->border);
    disable_walls(this->border);
    this->border.north_enabled = true;
    this->border.north = Glyph{L'─', foreground(Color::Blue)};
}
}  // namespace gol
