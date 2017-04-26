#include "twidgets.hpp"

class Bordered : public twf::Horizontal_layout {
   public:
    Bordered() {
        auto& tb = make_child<twf::Textbox>();
        tb.brush().set_background(twf::Color::Light_blue);
        tb.enable_border();
        this->brush().set_foreground(twf::Color::Red);
        this->enable_border();
    }
};

int main() {
    twf::System sys;

    Bordered mw;
    // twf::Textbox mw;
    // mw.enable_border();
    // mw.brush().set_background(twf::Color::Light_blue);

    sys.set_head(&mw);

    return sys.run();
}
