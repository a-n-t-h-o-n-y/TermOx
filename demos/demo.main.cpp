#include <cppurses/system/system.hpp>

#include "demos.hpp"

int main() { return cppurses::System{}.run<demos::Demos>(); }
