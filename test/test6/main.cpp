#include "twidgets.hpp"
#include <string>

class Meta : public twf::Vertical_layout {
   public:
    Meta() { this->make_child<twf::Titlebar>("Notes"); }
};

int main() {
    twf::System sys;

    Meta w;
    auto& tb = w.make_child<twf::Textbox>();
    tb.set_background(twf::Color::Light_gray);
    tb.background_tile() = twf::Glyph(" ", twf::Attribute::Underline);

    sys.set_head(&w);

    return sys.run();
}
