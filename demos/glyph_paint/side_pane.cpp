#include "side_pane.hpp"

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/widget/widget_free_functions.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

using namespace cppurses;

namespace demos {
namespace glyph_paint {

Side_pane::Side_pane() {
    this->width_policy.fixed(16);

    space1.wallpaper = L'─';
    space2.wallpaper = L'─';

    glyph_select.height_policy.preferred(6);

    color_select_stack.height_policy.fixed(3);
    set_background_recursive(color_select_stack.top_row, Color::Light_gray);
    set_foreground_recursive(color_select_stack.top_row, Color::Black);

    show_glyph.height_policy.fixed(1);
    show_glyph.set_alignment(Alignment::Center);
}
}  // namespace glyph_paint
}  // namespace demos
