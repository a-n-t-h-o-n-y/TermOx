#include <twidgets.hpp>

#include <functional>

class Bordered : public twf::Horizontal_layout {
   public:
    Bordered() {
        this->make_child<twf::Textbox>();
        this->enable_border();
    }
};

// class Checked : public twf::Horizontal_layout {
//     public:
//         Checked() {
//             this->make_child<twf::Checkbox>();
//         }
// };

int main() {
    twf::System s;

    twf::Horizontal_layout w;
    auto& ckbx = w.make_child<twf::Checkbox>();
    auto change_color = [&ckbx](twf::Color c) {
        ckbx.brush().set_background(c);
    };
    ckbx.checked.connect(std::bind(change_color, twf::Color::Light_blue));
    ckbx.unchecked.connect(std::bind(change_color, twf::Color::Light_green));

    s.set_head(&w);
    return s.run();
}

// int main() {
//     twf::System s;

//     twf::Border b;
//     b.enable();
//     b.disable_corners();
//     b.enable_north_west();
//     b.enable_south_east();
//     b.enable_south_west();
//     b.disable_east();
//     b.disable_north();
//     b.set_north_west("│");
//     b.set_south_east("─");

//     Bordered widg;
//     widg.border() = b;
//     auto& c = widg.make_child<Bordered>();
//     c.border() = b;

//     s.set_head(&widg);
//     return s.run();
// }
