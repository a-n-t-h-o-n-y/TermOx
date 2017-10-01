#include "cppurses.hpp"

using namespace cppurses;

int main() {
    System sys;
    Horizontal_layout hl;
    auto& menu = hl.make_child<Menu>("~ Set Background Color ~");
    set_foreground(menu, Color::Red);
    enable_border(menu);

    menu.make_item("Violet", [&menu] { set_background(menu, Color::Violet); });
    menu.make_item(Glyph_string{"Light Blue", foreground(Color::Yellow)},
                   [&menu] { set_background(menu, Color::Light_blue); });
    menu.make_item("White", [&menu] { set_background(menu, Color::White); });
    menu.make_item("Remove item", slot::remove_item(menu, 1));
    menu.make_item("Add Item", slot::make_item(menu, "N e w  I t e m", [&menu] {
                       set_background(menu, Color::Light_gray);
                   }));

    sys.set_head(&hl);
    Focus::set_focus_to(&menu);

    return sys.run();
}
