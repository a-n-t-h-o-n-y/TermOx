#include <cppurses/system/system.hpp>

#include "demo_menu.hpp"

int main() { return cppurses::System{}.run<demos::Demos>(); }
