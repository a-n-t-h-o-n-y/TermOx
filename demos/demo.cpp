#include <chrono>
#include <iostream>
#include <thread>

#include <cppurses/cppurses.hpp>

#include "main_menu.hpp"

int main() {
    cppurses::System sys;
    demos::Main_menu demo_menu;
    Tree::set_head(&demo_menu);
    Focus::set_focus_to(&demo_menu.main_menu.menu());
    return sys.run();
}
