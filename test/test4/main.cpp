#include "vertical_layout_widget.hpp"
#include <twidgets.hpp>

#include <stdexcept>
#include <iostream>

int main() {
    twf::System sys;
    Vertical_test vt;
    sys.set_head(&vt);
    return sys.run();
}
