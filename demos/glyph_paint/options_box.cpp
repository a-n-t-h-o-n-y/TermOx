#include "options_box.hpp"

#include <cppurses/cppurses.hpp>

using namespace cppurses;

namespace demos {
namespace glyph_paint {

Options_A::Options_A() {
    set_background(clone_btn, Color::White);
    set_foreground(clone_btn, Color::Black);

    set_background(clear_btn, Color::White);
    set_foreground(clear_btn, Color::Black);

    set_background(more_btn, Color::Light_gray);
    set_foreground(more_btn, Color::Black);
}

Options_B::Options_B() {
    space1.background_tile = "─";

    save_layout.height_policy.type(Size_policy::Fixed);
    save_layout.height_policy.hint(1);

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

    set_background(back_btn, Color::Light_gray);
    set_foreground(back_btn, Color::Black);

    set_background(palette_box, Color::White);
    set_foreground(palette_box, Color::Black);
}

Options_stack::Options_stack() {
    this->set_active_page(0);
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(6);

    options_a.more_btn.clicked.connect(
        cppurses::slot::set_active_page(*this, 1));

    options_b.back_btn.clicked.connect(
        cppurses::slot::set_active_page(*this, 0));
}

}  // namespace glyph_paint
}  // namespace demos
