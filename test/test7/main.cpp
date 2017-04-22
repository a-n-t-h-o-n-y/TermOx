#include <twidgets.hpp>

class Meta_textbox : public twf::Textbox {
   public:
    Meta_textbox() {}

    bool paint_event(const twf::Paint_event& e) override {
        std::stringstream ss;
        ss << "Current width: " << this->geometry().width()
           << "\nInner width: " << this->width() << "\nHorizontal size_policy: "
           << this->geometry().size_policy().horizontal_policy
           << "\nHorizontal size_hint: " << this->geometry().width_hint()
           << "\nHorizontal stretch: "
           << this->geometry().size_policy().horizontal_stretch
           << "\nMinimum Width: " << this->geometry().min_width()
           << "\nMaximum Width: " << this->geometry().max_width() << "\n";
        this->contents_ = ss.str();
        this->lower_bound_ = ss.str().size();
        return Textbox::paint_event(e);
    }
};

class Split_widg : public twf::Horizontal_layout {
   public:
    Split_widg() {
        auto& box1 = this->make_child<Meta_textbox>();
        box1.brush().set_foreground(twf::Color::Light_blue);
        box1.geometry().size_policy().horizontal_policy =
            twf::Size_policy::Fixed;
        // box1.geometry().set_max_width(30);
        // box1.geometry().set_min_width(10);
        box1.geometry().set_width_hint(20);
        box1.brush().set_background(twf::Color::Light_green);

        auto& box2 = this->make_child<Meta_textbox>();
        box2.geometry().size_policy().horizontal_policy =
            twf::Size_policy::Preferred;
        // box2.enable_border();
        box2.geometry().set_width_hint(35);
        // box2.geometry().size_policy().horizontal_policy =
        //     twf::Size_policy::Expanding;
        box2.brush().set_foreground(twf::Color::Light_green);
        box2.brush().set_background(twf::Color::Light_blue);
        box2.geometry().set_max_width(45);
        box2.geometry().set_min_width(10);
        // box2.geometry().size_policy().horizontal_stretch = 9;

        auto& box3 = this->make_child<Meta_textbox>();
        box3.geometry().size_policy().horizontal_policy =
            twf::Size_policy::Fixed;
        box3.geometry().set_width_hint(20);
        box3.geometry().set_min_width(10);
        box3.brush().set_background(twf::Color::White);
        box3.brush().set_foreground(twf::Color::Black);

        // box3.geometry().set_width_hint(20);
        // box1.geometry().set_min_width(7);

        // auto& box4 = this->make_child<Meta_textbox>();
        // box4.geometry().size_policy().horizontal_policy =
        //     twf::Size_policy::Fixed;
        // box4.geometry().set_width_hint(20);
    }
};

int main() {
    twf::System sys;

    Split_widg sw;
    sw.enable_border();

    sys.set_head(&sw);

    return sys.run();
}
