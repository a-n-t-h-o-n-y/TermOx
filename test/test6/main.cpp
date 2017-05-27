#include "twidgets.hpp"
#include <string>

#include <aml/optional/optional.hpp>

class Meta : public twf::Vertical_layout {
   public:
    Meta() { this->make_child<twf::Titlebar>("ƴots"); }
};

class scroll_test : public twf::Widget {
   public:
    bool mouse_press_event(const twf::Mouse_event& event) override {
        if (event.button() == twf::Mouse_button::ScrollUp) {
            up_ = true;
        } else if (event.button() == twf::Mouse_button::ScrollDown) {
            up_ = false;
        }
        this->update();
        return true;
    }

    bool paint_event(const twf::Paint_event& event) override {
        twf::Painter p{this};
        if (up_) {
            p.put(twf::Glyph_string("Scroll Up", twf::Attribute::Bold,
                                    twf::Attribute::Underline,
                                    twf::background(twf::Color::Red),
                                    twf::foreground(twf::Color::Light_blue)));
        } else {
            p.put("Scroll Down");
        }
        return Widget::paint_event(event);
    }

   private:
    bool up_ = false;
};

class resizing_button : public twf::Push_button {
   public:
    resizing_button() : Push_button("resize") {
        clicked.connect([this] {
            this->geometry().size_policy().vertical_policy =
                twf::Size_policy::Fixed;
            this->geometry().set_height_hint(3);
        });
        this->enable_border();
        this->set_background(twf::Color::Violet);
    }
};

int main() {
    twf::System sys;

    Meta w;
    auto& tb = w.make_child<twf::Textbox>();
    tb.geometry().size_policy().vertical_policy = twf::Size_policy::Expanding;
    tb.set_background(twf::Color::Light_gray);
    tb.set_background_tile(twf::Glyph(" ", twf::Attribute::Underline));
    tb.enable_border();
    tb.border().set_north_west("╭");
    tb.border().set_north_east("╮");
    tb.border().set_south_west("╰");
    tb.border().set_south_east("╯");
    w.make_child<resizing_button>();
    w.make_child<scroll_test>();
    auto& pb = w.make_child<twf::Push_button>("delete textbox");
    pb.geometry().set_height_hint(3);
    pb.geometry().size_policy().vertical_policy = twf::Size_policy::Preferred;
    pb.clicked.connect(tb.close);
    pb.enable_border();
    pb.border().set_north_west("╭");
    pb.border().set_north_east("╮");
    pb.border().set_south_west("╰");
    pb.border().set_south_east("╯");

    sys.set_head(&w);

    return sys.run();
}
