#include "vertical_layout_widget.hpp"

int main() {
    twf::System sys;
    Vertical_test vt;
    sys.set_head(&vt);
    return sys.run();
}
