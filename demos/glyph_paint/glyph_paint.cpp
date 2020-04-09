#include "glyph_paint.hpp"

#include <cppurses/widget/widget_slots.hpp>

using namespace cppurses;

namespace demos::glyph_paint {

Glyph_paint::Glyph_paint()
{
    side_pane.glyph_select.make_connections(slot::set_symbol(paint_area));

    side_pane.color_pages.foreground.color_selected.connect(
        slot::set_foreground_color(paint_area));
    side_pane.color_pages.background.color_selected.connect(
        slot::set_background_color(paint_area));

    side_pane.attribute_box.top_row.bold_box.wrapped.checked.connect(
        slot::set_attribute(paint_area, Attribute::Bold));
    side_pane.attribute_box.top_row.bold_box.wrapped.unchecked.connect(
        slot::remove_attribute(paint_area, Attribute::Bold));

    side_pane.attribute_box.top_row.dim_box.wrapped.checked.connect(
        slot::set_attribute(paint_area, Attribute::Dim));
    side_pane.attribute_box.top_row.dim_box.wrapped.unchecked.connect(
        slot::remove_attribute(paint_area, Attribute::Dim));

    side_pane.attribute_box.inverse_box.wrapped.checked.connect(
        slot::set_attribute(paint_area, Attribute::Inverse));
    side_pane.attribute_box.inverse_box.wrapped.unchecked.connect(
        slot::remove_attribute(paint_area, Attribute::Inverse));

    side_pane.attribute_box.italic_box.wrapped.checked.connect(
        slot::set_attribute(paint_area, Attribute::Italic));
    side_pane.attribute_box.italic_box.wrapped.unchecked.connect(
        slot::remove_attribute(paint_area, Attribute::Italic));

    side_pane.attribute_box.underline_box.wrapped.checked.connect(
        slot::set_attribute(paint_area, Attribute::Underline));
    side_pane.attribute_box.underline_box.wrapped.unchecked.connect(
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
    side_pane.options_box.options_a.cursor_box.checkbox.toggled.connect(
        cppurses::slot::toggle_cursor(paint_area));
    paint_area.erase_disabled.connect(cppurses::slot::uncheck(
        side_pane.options_box.options_a.eraser_box.checkbox));
    paint_area.erase_enabled.connect(cppurses::slot::check(
        side_pane.options_box.options_a.eraser_box.checkbox));
    side_pane.options_box.options_a.eraser_box.checkbox.checked.connect(
        slot::enable_erase(paint_area));
    side_pane.options_box.options_a.eraser_box.checkbox.unchecked.connect(
        slot::disable_erase(paint_area));
    side_pane.options_box.options_a.grid_box.checkbox.checked.connect(
        slot::enable_grid(paint_area));
    side_pane.options_box.options_a.grid_box.checkbox.unchecked.connect(
        slot::disable_grid(paint_area));
    side_pane.options_box.options_b.save_file.save_requested.connect(
        slot::write(paint_area));
    side_pane.options_box.options_b.open_file.open_requested.connect(
        slot::read(paint_area));
}

}  // namespace demos::glyph_paint
