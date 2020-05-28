#include "options_box.hpp"

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

using namespace cppurses;

namespace demos {
namespace glyph_paint {

Options_A::Options_A()
{
    using namespace cppurses::pipe;
    clone_btn | bg(Color::White) | fg(Color::Black);
    clear_btn.main_btn | bg(Color::White) | fg(Color::Black);
    more_btn | bg(Color::Light_gray) | fg(Color::Black);
}

Options_B::Options_B()
{
    space1.set_wallpaper(L'─');

    palette_label.set_alignment(Align::Center);
    // palette_box.add_option("Dawn Bringer").connect([] {
    //     System::terminal.set_color_palette(Palettes::DawnBringer());
    // });
    // palette_box.add_option("Standard").connect([] {
    //     System::terminal.set_color_palette(Palettes::Standard());
    // });

    back_btn.brush.set_background(Color::Light_gray);
    back_btn.brush.set_foreground(Color::Black);

    palette_box.brush.set_background(Color::White);
    palette_box.brush.set_foreground(Color::Black);
}

Options_stack::Options_stack()
{
    this->set_active_page(0);
    this->height_policy.maximum(6);

    options_a.more_btn.pressed.connect(
        cppurses::slot::set_active_page(*this, 1));

    options_b.back_btn.pressed.connect(
        cppurses::slot::set_active_page(*this, 0));
}
}  // namespace glyph_paint
}  // namespace demos
