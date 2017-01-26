#include <twidgets.hpp>

class Holder: public twf::Horizontal_layout {
    public:
    Holder() {
        auto& w = this->make_child<twf::Textbox>();
        // w.enable_border();
        w.brush().set_background(twf::Color::Light_blue);
        w.brush().set_foreground(twf::Color::Orange);
        w.brush().add_attributes(twf::Attribute::Bold);
    }
};

int main() {
    twf::System sys;

    Holder mw;

    sys.set_head(&mw);

    return sys.run();
}
