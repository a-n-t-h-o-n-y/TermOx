#include "main_widget.hpp"
#include "color_test_widget.hpp"

#include <twidgets.hpp>

class Layout_color : public twf::Horizontal_layout {
    public:
        Layout_color() {
            // this->make_child<Exit_widget>();
            this->make_child<Color_test_widget>();
            auto& cw = this->make_child<Color_test_widget>();
            cw.size_policy().horizontal_stretch = 2;
            cw.brush().set_background(Color::Light_green);
        }
};

int main() {
    twf::System sys;

    // Main_widget mw;
    // Color_test_widget mw;
    Layout_color mw;

    sys.set_head(&mw);

    return sys.run();
}
