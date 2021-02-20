#include <termox/system/system.hpp>

#include "demo_menu.hpp"

int main() { return ox::System{ox::Mouse_mode::Drag}.run<demos::Demos>(); }
