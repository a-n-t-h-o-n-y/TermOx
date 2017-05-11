#include "twidgets.hpp"
#include <string>

class Meta : public twf::Vertical_layout {
   public:
    Meta() { this->make_child<twf::Titlebar>("Notes"); }
};

int main() {
    twf::System sys;

    Meta w;
    w.make_child<twf::Textbox>().set_background(twf::Color::Light_gray);

    sys.set_head(&w);

    return sys.run();
}
