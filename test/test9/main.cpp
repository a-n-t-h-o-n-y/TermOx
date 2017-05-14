#include <twidgets.hpp>
#include <functional>

class Bordered : public twf::Horizontal_layout {
   public:
    Bordered() {
        this->make_child<twf::Textbox>();
        this->enable_border();
    }
};

// int main() {
//     twf::System s;

//     twf::Horizontal_layout w;
//     auto& ckbx = w.make_child<twf::Checkbox>();
//     auto& vl = w.make_child<twf::Vertical_layout>();
//     vl.make_child<twf::Titlebar>("~ Checkbox and Textbox ~");
//     auto& tb = vl.make_child<twf::Textbox>();
//     tb.enable_border();
//     auto change_color = [&tb](twf::Color c) {
//         tb.brush().set_background(c);
//         tb.update();
//     };
//     ckbx.checked.connect(std::bind(change_color, twf::Color::Light_blue));
//     ckbx.unchecked.connect(std::bind(change_color, twf::Color::Light_green));
//     auto& sb = vl.make_child<twf::Horizontal_scrollbar>();
//     sb.left.connect(std::bind(change_color, twf::Color::Brown));
//     sb.right.connect(std::bind(change_color, twf::Color::Violet));

//     s.set_head(&w);
//     return s.run();
// }

int main() {
    twf::System s;

    twf::Border b;
    b.enable();
    b.disable_corners();
    b.enable_north_west();
    b.enable_south_east();
    b.enable_south_west();
    b.disable_east();
    b.disable_north();
    b.set_north_west("│");
    b.set_south_east("─");

    Bordered widg;
    widg.border() = b;
    auto& c = widg.make_child<Bordered>();
    c.border() = b;

    s.set_head(&widg);
    return s.run();
}
