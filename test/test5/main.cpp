#include <twidgets.hpp>
#include <aml/signals/slot.hpp>

#include <fstream>

class Holder : public twf::Horizontal_layout {
   public:
    Holder() {
        auto& w = this->make_child<twf::Textbox>();
        w.enable_border();
        w.brush().set_background(twf::Color::Black);
        w.brush().set_foreground(twf::Color::Light_green);
        // w.brush().add_attributes(twf::Attribute::Underline);
    }
};

class Main_widget : public twf::Vertical_layout {
   public:
    Main_widget() {
        auto& tb = this->make_child<twf::Textbox>();
        tb.brush().set_background(twf::Color::Black);
        tb.brush().set_foreground(twf::Color::Light_green);
        tb.size_policy().vertical_stretch = 4;

        auto& hl = this->make_child<twf::Horizontal_layout>();
        auto& v1 = hl.make_child<twf::Vertical_layout>();
        v1.disable_cursor();
        v1.enable_border();
        twf::Brush brush;
        brush.set_foreground(twf::Color::Red);
        brush.set_background(twf::Color::Red);
        v1.brush() = brush;
        // v1.brush().set_foreground(twf::Color::Dark_red);
        // v1.brush().set_background(twf::Color::Dark_red);
        auto& btn = hl.make_child<twf::Push_button>("Save");
        auto& v2 = hl.make_child<twf::Vertical_layout>();
        v2.disable_cursor();
        v2.enable_border();
        v2.brush() = brush;
        auto save_text = [&tb](){
            std::ofstream fs{"test2.txt"};
            fs << tb.text();
        };
        btn.clicked.connect(save_text);
    }
};

int main() {
    twf::System sys;

    Main_widget mw;

    sys.set_head(&mw);

    return sys.run();
}
