#include "glyph_paint.hpp"

#include <cppurses/widget/widget_slots.hpp>

using namespace cppurses;

namespace demos {
namespace glyph_paint {

Glyph_paint::Glyph_paint() {
    this->set_name("Glyph_paint - main demo widget");
    side_pane.glyph_select.make_connections(slot::set_symbol(paint_area));

    side_pane.color_select_foreground.color_changed.connect(
        slot::set_foreground_color(paint_area));
    side_pane.color_select_background.color_changed.connect(
        slot::set_background_color(paint_area));

    side_pane.attribute_box.bold_box.checked.connect(
        slot::set_attribute(paint_area, Attribute::Bold));
    side_pane.attribute_box.bold_box.unchecked.connect(
        slot::remove_attribute(paint_area, Attribute::Bold));

    side_pane.attribute_box.dim_box.checked.connect(
        slot::set_attribute(paint_area, Attribute::Dim));
    side_pane.attribute_box.dim_box.unchecked.connect(
        slot::remove_attribute(paint_area, Attribute::Dim));

    side_pane.attribute_box.inverse_box.checked.connect(
        slot::set_attribute(paint_area, Attribute::Inverse));
    side_pane.attribute_box.inverse_box.unchecked.connect(
        slot::remove_attribute(paint_area, Attribute::Inverse));

    side_pane.attribute_box.italic_box.checked.connect(
        slot::set_attribute(paint_area, Attribute::Italic));
    side_pane.attribute_box.italic_box.unchecked.connect(
        slot::remove_attribute(paint_area, Attribute::Italic));

    side_pane.attribute_box.underline_box.checked.connect(
        slot::set_attribute(paint_area, Attribute::Underline));
    side_pane.attribute_box.underline_box.unchecked.connect(
        slot::remove_attribute(paint_area, Attribute::Underline));

    paint_area.glyph_changed.connect(
        cppurses::slot::update_status(side_pane.show_glyph));
    side_pane.options_box.options_a.clone_btn.clicked.connect(
        slot::toggle_clone(paint_area));
    side_pane.options_box.options_a.clone_btn.clicked.connect(
        cppurses::slot::update_status(
            side_pane.show_glyph,
            Glyph_string{"Clone", foreground(Color::Light_gray)}));
    side_pane.options_box.options_a.clear_btn.clicked.connect(
        slot::clear(paint_area));
    side_pane.options_box.options_a.cursor_box.toggled.connect(
        cppurses::slot::toggle_cursor(paint_area));
    paint_area.erase_disabled.connect(
        cppurses::slot::uncheck(side_pane.options_box.options_a.eraser_box));
    paint_area.erase_enabled.connect(
        cppurses::slot::check(side_pane.options_box.options_a.eraser_box));
    side_pane.options_box.options_a.eraser_box.checked.connect(
        slot::enable_erase(paint_area));
    side_pane.options_box.options_a.eraser_box.unchecked.connect(
        slot::disable_erase(paint_area));
    side_pane.options_box.options_a.grid_box.checked.connect(
        slot::enable_grid(paint_area));
    side_pane.options_box.options_a.grid_box.unchecked.connect(
        slot::disable_grid(paint_area));
    side_pane.options_box.options_b.save_file.save_requested.connect(
        slot::write(paint_area));
    side_pane.options_box.options_b.open_file.open_requested.connect(
        slot::read(paint_area));
}

}  // namespace glyph_paint
}  // namespace demos
