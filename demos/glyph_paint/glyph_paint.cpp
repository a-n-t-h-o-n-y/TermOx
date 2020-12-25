#include "glyph_paint.hpp"

#include <termox/widget/widget_slots.hpp>

using namespace ox;

namespace paint {

Glyph_paint::Glyph_paint()
{
    side_pane.glyph_selector.selected.connect(slot::set_symbol(paint_area));

    side_pane.color_pages.foreground.color_selected.connect(
        slot::set_foreground_color(paint_area));
    side_pane.color_pages.background.color_selected.connect(
        slot::set_background_color(paint_area));

    side_pane.trait_box.top_row.bold_box.checkbox.checked.connect(
        slot::set_trait(paint_area, Trait::Bold));
    side_pane.trait_box.top_row.bold_box.checkbox.unchecked.connect(
        slot::remove_traits(paint_area, Trait::Bold));

    side_pane.trait_box.top_row.dim_box.checkbox.checked.connect(
        slot::set_trait(paint_area, Trait::Dim));
    side_pane.trait_box.top_row.dim_box.checkbox.unchecked.connect(
        slot::remove_traits(paint_area, Trait::Dim));

    side_pane.trait_box.inverse_box.checkbox.checked.connect(
        slot::set_trait(paint_area, Trait::Inverse));
    side_pane.trait_box.inverse_box.checkbox.unchecked.connect(
        slot::remove_traits(paint_area, Trait::Inverse));

    side_pane.trait_box.italic_box.checkbox.checked.connect(
        slot::set_trait(paint_area, Trait::Italic));
    side_pane.trait_box.italic_box.checkbox.unchecked.connect(
        slot::remove_traits(paint_area, Trait::Italic));

    side_pane.trait_box.underline_box.checkbox.checked.connect(
        slot::set_trait(paint_area, Trait::Underline));
    side_pane.trait_box.underline_box.checkbox.unchecked.connect(
        slot::remove_traits(paint_area, Trait::Underline));

    paint_area.glyph_changed.connect(
        ox::slot::update_status(side_pane.show_glyph));
    side_pane.options_box.options_a.clone_btn.pressed.connect(
        slot::toggle_clone(paint_area));
    side_pane.options_box.options_a.clone_btn.pressed.connect(
        ox::slot::update_status(side_pane.show_glyph,
                                Glyph_string{L"Clone", fg(Color::Light_gray)}));
    side_pane.options_box.options_a.clear_btn.pressed.connect(
        slot::clear(paint_area));
    side_pane.options_box.options_a.cursor_box.checkbox.toggled.connect(
        ox::slot::toggle_cursor(paint_area));
    paint_area.erase_disabled.connect(
        ox::slot::uncheck(side_pane.options_box.options_a.eraser_box.checkbox));
    paint_area.erase_enabled.connect(
        ox::slot::check(side_pane.options_box.options_a.eraser_box.checkbox));
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

}  // namespace paint
