#include "options_box.hpp"
#include <cppurses/cppurses.hpp>

using namespace cppurses;

namespace demos {
namespace glyph_paint {

Options_box::Options_box() {
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(9);

    space1.background_tile = "─";

    clone_btn.height_policy.type(Size_policy::Fixed);
    clone_btn.height_policy.hint(1);
    set_background(clone_btn, Color::Gray);
    set_foreground(clone_btn, Color::Black);

    save_layout.height_policy.type(Size_policy::Fixed);
    save_layout.height_policy.hint(1);

    clear_btn.height_policy.type(Size_policy::Fixed);
    clear_btn.height_policy.hint(1);
    set_background(clear_btn, Color::Gray);
    set_foreground(clear_btn, Color::Black);

    palette_label.set_alignment(Alignment::Center);
    palette_box.add_option("Dawn Bringer").connect([] {
        System::set_palette(std::make_unique<DawnBringer_palette>());
    });
    palette_box.add_option("Standard").connect([] {
        System::set_palette(std::make_unique<Standard_palette>());
    });

    save_btn.width_policy.type(Size_policy::Fixed);
    save_btn.width_policy.hint(4);
    set_background(save_btn, Color::Blue);
    save_seperator.background_tile = "⏵";
    set_background(save_filename_edit, Color::White);
    set_foreground(save_filename_edit, Color::Black);
    save_filename_edit.set_ghost_color(Color::Dark_gray);

    open_btn.width_policy.type(Size_policy::Fixed);
    open_btn.width_policy.hint(4);
    set_background(open_btn, Color::Blue);
    open_seperator.background_tile = "⏵";
    set_background(open_filename_edit, Color::White);
    set_foreground(open_filename_edit, Color::Black);
    open_filename_edit.set_ghost_color(Color::Dark_gray);
}

}  // namespace glyph_paint
}  // namespace demos
