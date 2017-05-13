#include "twidgets.hpp"
#include <string>

#include <aml/optional/optional.hpp>

class Meta : public twf::Vertical_layout {
   public:
    Meta() { this->make_child<twf::Titlebar>("ƴots"); }
};

int main() {
    twf::System sys;

    Meta w;
    auto& tb = w.make_child<twf::Textbox>();
    tb.geometry().size_policy().vertical_policy = twf::Size_policy::Expanding;
    tb.set_background(twf::Color::Light_gray);
    tb.background_tile() = twf::Glyph(" ", twf::Attribute::Underline);
    auto& pb = w.make_child<twf::Push_button>("delete textbox");
    pb.geometry().set_height_hint(3);
    pb.geometry().size_policy().vertical_policy = twf::Size_policy::Preferred;
    pb.clicked.connect(tb.close);

    // you need delete_later to check if it is valid? though its object will be
    // gone,
    // the reason you are having trouble is because you hold a reference, you
    // need to
    // ask the parent for a copy, if it exists, maybe by name, maybe it returns
    // an optional?
    // you want to make it simple to use.

    sys.set_head(&w);

    return sys.run();
}
