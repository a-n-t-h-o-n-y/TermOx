#include "glyph_paint.hpp"

namespace demos {
namespace glyph_paint {

Glyph_paint::Glyph_paint() {
    side_pane.glyph_select.make_connections(slot::set_symbol(paint_area));

    side_pane.color_select_foreground.color_changed.connect(
        slot::set_foreground_color(paint_area));
    side_pane.color_select_background.color_changed.connect(
        slot::set_background_color(paint_area));

    side_pane.options_box.eraser_box.checked.connect(
        [this] { this->to_eraser(); });
    side_pane.options_box.eraser_box.unchecked.connect(
        [this] { this->from_eraser(); });

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

    side_pane.options_box.clone_btn.clicked.connect(
        slot::toggle_clone(paint_area));

    side_pane.options_box.clone_btn.clicked.connect(
        cppurses::slot::update_status(
            side_pane.show_glyph,
            Glyph_string{"Clone", foreground(Color::Light_gray)}));

    side_pane.options_box.clear_btn.clicked.connect(slot::clear(paint_area));

    side_pane.options_box.cursor_box.toggled.connect(
        cppurses::slot::toggle_cursor(paint_area));
}

void Glyph_paint::to_eraser() {
    before_eraser_ = paint_area.glyph();
    paint_area.set_glyph(" ");
}

void Glyph_paint::from_eraser() {
    paint_area.set_glyph(before_eraser_);
}

}  // namespace glyph_paint
}  // namespace demos
