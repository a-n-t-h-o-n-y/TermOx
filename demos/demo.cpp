#include <cppurses/cppurses.hpp>

#include "main_menu.hpp"

int main() {
    cppurses::System sys;
    demos::Main_menu mm_widget;

    cppurses::Tree::set_head(&mm_widget);
    Focus::set_focus_to(&(mm_widget.main_menu));
    return sys.run();
}
