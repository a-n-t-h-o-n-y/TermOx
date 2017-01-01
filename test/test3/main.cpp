#include "main_widget.hpp"

#include <twidgets.hpp>

int main() {
    twf::System sys;

    Main_widget mw;

    sys.set_head(&mw);

    return sys.run();
}
