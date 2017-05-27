#include <twidgets.hpp>

class Split_widg : public twf::Horizontal_layout {
   public:
    Split_widg() {
        this->enable_border();
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
        sb.up.connect(tb.scroll_up);
        sb.down.connect(tb.scroll_down);
    }
};

class H_scrolls : public twf::Vertical_layout {
   public:
    H_scrolls() {
        auto& tb = this->make_child<twf::Textbox>();
        auto& sb = this->make_child<twf::Horizontal_scrollbar>();
        sb.left.connect(tb.scroll_up);
        sb.right.connect(tb.scroll_down);
    }
};

class Clicker : public twf::Widget {
   public:
    Clicker() { this->disable_cursor(); }
    bool mouse_press_event(const twf::Mouse_event& event) override {
        if (event.button() == twf::Mouse_button::Left) {
            pos_.x = event.local_x();
            pos_.y = event.local_y();
            this->update();
        }
        return Widget::mouse_press_event(event);
    }

    bool paint_event(const twf::Paint_event& event) override {
        twf::Painter p{this};
        p.put('X', pos_.x, pos_.y);
        return Widget::paint_event(event);
    }

   private:
    twf::Coordinate pos_;
};

class L_clicker : public twf::Horizontal_layout {
   public:
    L_clicker() {
        this->enable_border();
        this->make_child<Clicker>();
        this->make_child<Clicker>().enable_border();
    }
};

int main() {
    twf::System sys;

    twf::Vertical_layout vl;
    vl.make_child<twf::Titlebar>("Text Views");
    auto& hl = vl.make_child<twf::Horizontal_layout>();
    auto& tb_1 = hl.make_child<twf::Textbox>();
    auto& tb_2 = hl.make_child<twf::Textbox_base>();
    tb_1.enable_border();
    tb_2.enable_border();
    tb_2.disable_word_wrap();

    // Signals and Slots
    tb_1.text_changed.connect(tb_2.set_text);

    auto& cc = vl.make_child<twf::Color_chooser>();
    cc.color_changed.connect(tb_1.set_background);
    cc.size_policy().horizontal_policy = twf::Size_policy::Preferred;

    // vl.make_child<Split_widg>();
    // auto& tb = vl.make_child<twf::Textbox_base>("Hello,\nWorld!");
    // auto& s_up = vl.make_child<twf::Push_button>("scroll up");
    // auto& s_down = vl.make_child<twf::Push_button>("scroll down");

    // s_up.clicked.connect(tb.scroll_up);
    // s_down.clicked.connect(tb.scroll_down);

    sys.set_head(&vl);

    return sys.run();
}
