#include <termox/termox.hpp>

#include "demo_menu.hpp"

int main() { return ox::System{ox::Mouse_mode::Drag}.run<demo::Demos>(); }
