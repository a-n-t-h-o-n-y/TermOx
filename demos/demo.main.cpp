#include <termox/system/system.hpp>

#include "demo_menu.hpp"

int main() { return ox::System{}.run<demos::Demos>(); }
