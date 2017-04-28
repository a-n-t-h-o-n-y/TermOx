#include <twidgets.hpp>

class Meta_textbox : public twf::Textbox {
   public:
    Meta_textbox() {}

    bool paint_event(const twf::Paint_event& e) override {
        std::stringstream ss;
        ss << "width: " << this->geometry().width()
           << "\nwidth[inner]: " << this->width()
           << "\nHorizontal size_policy: "
           << this->geometry().size_policy().horizontal_policy
           << "\nh_size_hint: " << this->geometry().width_hint()
           << "\nh_stretch: "
           << this->geometry().size_policy().horizontal_stretch
           << "\nmin_width: " << this->geometry().min_width()
           << "\nmax_width: " << this->geometry().max_width() << "\n";
        this->contents_ = ss.str();
        this->lower_bound_ = ss.str().size();
        return Textbox::paint_event(e);
    }
};

// widg.geometry().size_policy().horizontal_policy = twf::Size_policy::Fixed;
// widg.geometry().size_policy().horizontal_stretch = 2;
// widg.geometry().set_width_hint();
// widg.geometry().set_max_width();
// widg.geometry().set_min_width();

// widg.brush().set_foreground(twf::Color::Light_blue);
// widg.brush().set_background(twf::Color::Light_green);
// widg.enable_border();

// auto& widg = this->make_child<Meta_textbox>();

class Split_widg : public twf::Horizontal_layout {
   public:
    Split_widg() {
        // auto& box1 = this->make_child<Meta_textbox>();
        // box1.enable_border();
        // box1.brush().set_background(twf::Color::Light_gray);
        // box1.brush().set_foreground(twf::Color::Dark_blue);
        // box1.geometry().size_policy().horizontal_policy =
        //     twf::Size_policy::Minimum;
        // box1.geometry().size_policy().vertical_policy =
        // twf::Size_policy::Fixed;
        // box1.geometry().set_height_hint(20);
        // box1.geometry().set_width_hint(17);
        // box1.geometry().set_min_width(3);

        // auto& box3 = this->make_child<Meta_textbox>();
        // box3.enable_border();
        // box3.brush().set_background(twf::Color::White);
        // box3.brush().set_foreground(twf::Color::Black);
        // box3.geometry().size_policy().horizontal_policy =
        //     twf::Size_policy::Maximum;
        // box3.geometry().set_width_hint(20);
        // box3.geometry().set_min_width(6);
        // box3.geometry().size_policy().vertical_policy =
        //     twf::Size_policy::Preferred;
        // box3.geometry().set_height_hint(30);

        // auto& box2 = this->make_child<Meta_textbox>();
        // box2.enable_border();
        // box2.brush().set_background(twf::Color::Green);
        // box2.brush().set_foreground(twf::Color::Light_blue);
        // box2.geometry().size_policy().horizontal_policy =
        //     twf::Size_policy::Minimum;
        // box2.geometry().set_width_hint(17);
        // box2.geometry().set_min_width(4);

        auto& tb = this->make_child<twf::Textbox>();
        tb.geometry().size_policy().horizontal_policy =
            twf::Size_policy::Expanding;
        tb.enable_border();
        tb.set_name("textbox");
        auto& sb = this->make_child<twf::Vertical_scrollbar>();
        sb.set_name("scrollbar");
        sb.up_button.set_name("up_button");
        sb.down_button.set_name("down_button");
        this->set_name("Layout");
        // auto& w = this->make_child<twf::Widget>();
        // w.brush().set_background(twf::Color::Light_blue);
        // this->enable_border();
    }
};

int main() {
    twf::System sys;

    Split_widg sw;
    // sw.enable_border();

    sys.set_head(&sw);

    return sys.run();
}
