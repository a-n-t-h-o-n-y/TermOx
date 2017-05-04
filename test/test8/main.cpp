#include <twidgets.hpp>

class Meta_textbox : public twf::Textbox {
   public:
    Meta_textbox() {}

    bool paint_event(const twf::Paint_event& e) override {
        std::stringstream ss; // ӌ
        ss << "widthӌ: " << this->geometry().width()
           << "\nwidth[inner]: " << this->width()
           << "\nHorizontal size_policy: "
           << this->geometry().size_policy().horizontal_policy
           << "\nh_size_hint: " << this->geometry().width_hint()
           << "\nh_stretch: "
           << this->geometry().size_policy().horizontal_stretch
           << "\nmin_width: " << this->geometry().min_width()
           << "\nmax_width: " << this->geometry().max_width();
        this->contents_ = ss.str();
        return Textbox::paint_event(e);
    }
};

class Meta_vert : public twf::Textbox {
   public:
    Meta_vert() {}

    bool paint_event(const twf::Paint_event& e) override {
        std::stringstream ss;
        ss << "height: " << this->geometry().height()
           << "\nheight[inner]: " << this->height()
           << "\nHorizontal size_policy: "
           << this->geometry().size_policy().horizontal_policy
           << "\nh_size_hint: " << this->geometry().height_hint()
           << "\nh_stretch: "
           << this->geometry().size_policy().horizontal_stretch
           << "\nmin_height: " << this->geometry().min_height()
           << "\nmax_height: " << this->geometry().max_height();
        this->contents_ = ss.str();
        return Textbox::paint_event(e);
    }
};

class Vert_thing : public twf::Vertical_layout {
   public:
    Vert_thing() {
        auto& box1 = this->make_child<Meta_vert>();
        box1.enable_border();
        box1.brush().set_background(twf::Color::Light_gray);
        box1.brush().set_foreground(twf::Color::Dark_blue);
        box1.geometry().set_height_hint(7);
        box1.geometry().size_policy().vertical_policy =
            twf::Size_policy::Minimum;

        auto& box3 = this->make_child<Meta_vert>();
        // box3.enable_border();
        box3.brush().set_background(twf::Color::White);
        box3.brush().set_foreground(twf::Color::Black);
        box3.geometry().set_height_hint(10);
        box3.geometry().size_policy().vertical_policy =
            twf::Size_policy::Maximum;

        auto& box2 = this->make_child<Meta_vert>();
        box2.enable_border();
        box2.brush().set_background(twf::Color::Green);
        box2.brush().set_foreground(twf::Color::Light_blue);
        box2.geometry().set_height_hint(7);
        box2.geometry().size_policy().vertical_policy =
            twf::Size_policy::Minimum;
    }
};

class Horiz_thing : public twf::Horizontal_layout {
   public:
    Horiz_thing() {
        auto& box1 = this->make_child<Meta_textbox>();
        box1.enable_border();
        box1.brush().set_background(twf::Color::Light_gray);
        box1.brush().set_foreground(twf::Color::Dark_blue);
        box1.geometry().size_policy().horizontal_policy =
            twf::Size_policy::Minimum;
        // box1.geometry().size_policy().vertical_policy = twf::Size_policy::Fixed;
        // box1.geometry().set_height_hint(5);
        box1.geometry().set_width_hint(5);
        box1.geometry().set_min_width(3);

        auto& box3 = this->make_child<Meta_textbox>();
        box3.enable_border();
        box3.brush().set_background(twf::Color::White);
        box3.brush().set_foreground(twf::Color::Black);
        box3.geometry().size_policy().horizontal_policy =
            twf::Size_policy::Fixed;
        box3.geometry().set_width_hint(10);
        // box3.geometry().set_min_width(6);
        // box3.geometry().size_policy().vertical_policy =
        //     twf::Size_policy::Fixed;
        // box3.geometry().set_height_hint(10);

        auto& box2 = this->make_child<Meta_textbox>();
        box2.enable_border();
        box2.brush().set_background(twf::Color::Green);
        box2.brush().set_foreground(twf::Color::Light_blue);
        box2.geometry().size_policy().horizontal_policy =
            twf::Size_policy::Minimum;
        box2.geometry().set_width_hint(5);
        box2.geometry().set_min_width(4);
    }
};

class Vert_horiz_thing : public twf::Vertical_layout {
    public:
        Vert_horiz_thing() {
            make_child<Horiz_thing>();
            make_child<Horiz_thing>();
            make_child<Horiz_thing>();
        }
};

int main() {
    twf::System s;

    Vert_horiz_thing vt;

    s.set_head(&vt);

    return s.run();
}
