#include "side_pane.hpp"
#include <cppurses/cppurses.hpp>

using namespace cppurses;

namespace demos {
namespace glyph_paint {

Side_pane::Side_pane() {
    space1.background_tile = L'─';
    space2.background_tile = L'─';

    this->width_policy.type(Size_policy::Fixed);
    this->width_policy.hint(16);

    glyph_select.height_policy.type(Size_policy::Preferred);
    glyph_select.height_policy.hint(6);

    color_select_stack.height_policy.type(Size_policy::Fixed);
    color_select_stack.height_policy.hint(3);
    set_background_recursive(color_select_stack.top_row, Color::Light_gray);
    set_foreground_recursive(color_select_stack.top_row, Color::Black);

    show_glyph.height_policy.type(Size_policy::Fixed);
    show_glyph.height_policy.hint(1);
    show_glyph.set_alignment(Alignment::Center);
}

}  // namespace glyph_paint
}  // namespace demos
