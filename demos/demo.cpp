#include "cppurses.hpp"
#include "main_menu.hpp"

int main() {
    cppurses::System sys;
    demo::Main_menu mm_widget;
    Focus::set_focus_to(&mm_widget.main_menu);

    sys.set_head(&mm_widget);
    return sys.run();
}
