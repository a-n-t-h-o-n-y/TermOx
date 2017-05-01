#include <twidgets.hpp>

class Split_widg : public twf::Horizontal_layout {
   public:
    Split_widg() {
        // Textbox
        auto& tb = this->make_child<twf::Textbox>();
        tb.geometry().size_policy().horizontal_policy =
            twf::Size_policy::Expanding;
        tb.enable_border();
        tb.set_name("textbox");

        // Scrollbar
        auto& sb = this->make_child<twf::Vertical_scrollbar>();
        sb.set_name("scrollbar");
        sb.up_button.set_name("up_button");
        sb.down_button.set_name("down_button");
        this->set_name("Layout");
        auto& w = this->make_child<twf::Widget>();
        w.brush().set_background(twf::Color::Light_blue);
        // this->enable_border();
    }
};

class Wid : public twf::Widget {
   public:
    bool paint_event(const twf::Paint_event& event) override {
        twf::Painter p{this};
        p.put_at(0, 0, "hekӍasd");
        return Widget::paint_event(event);
    }
};

int main() {
    twf::System sys;

    Split_widg sw;

    // Wid sw;

    sys.set_head(&sw);

    return sys.run();
}
