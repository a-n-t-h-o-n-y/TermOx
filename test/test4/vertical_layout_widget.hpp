#include <twidgets.hpp>

class Text_box : public twf::Widget {
   public:
    Text_box() {
        this->set_focus_policy(twf::Widget::Focus_policy::StrongFocus);
        this->set_cursor(true);
        this->enable_border();
        this->size_policy().horizontal_stretch = 2;
    }
    bool paint_event(const twf::Paint_event& event) override {
        // this->erase_widget_screen();
        twf::Painter p{this};
        p.move(0, 0);
        p.put(contents_);
        return Widget::paint_event(event);
    }
    bool key_press_event(const twf::Key_event& event) override {
        contents_.append(event.text());
        twf::Painter p{this};
        p.put(event.text());
        return Widget::key_press_event(event);
    }

   private:
    std::string contents_;
};

class Click_paint_widget : public twf::Widget {
   public:
    Click_paint_widget() {
        this->set_cursor(false);
        // this->enable_border();
    }

    bool paint_event(const twf::Paint_event& event) override {
        twf::Painter p{this};
        p.move(0, 0);
        p.put("Click Widget");
        return Widget::paint_event(event);
    }

    bool mouse_press_event(const twf::Mouse_event& event) override {
        twf::Painter p{this};
        p.move(event.local_x(), event.local_y());
        p.put("X");
        return Widget::mouse_press_event(event);
    }
};

class Vertical_test : public twf::Vertical_layout {
   public:
    Vertical_test() {
        auto& tb1 = this->make_child<Text_box>();
        // tb1.brush().set_background(twf::Color::Light_blue);
        // tb1.brush().set_foreground(twf::Color::Yellow);
        // tb1.border().set_south("Џ");
        tb1.border().disable_north();
        tb1.border().disable_north_west();
        tb1.border().disable_north_east();

        auto& hl = this->make_child<twf::Horizontal_layout>();
        auto& chl = hl.make_child<Text_box>();
        auto& vl = hl.make_child<Vertical_layout>();
        vl.make_child<Click_paint_widget>().enable_border();
        vl.make_child<Text_box>().brush().set_background(
            twf::Color::Light_blue);
        chl.brush().set_background(twf::Color::Light_gray);
        chl.brush().set_foreground(twf::Color::Red);
        chl.border().disable_walls();
        chl.border().enable_corners();

        // auto& tb1 = this->make_child<Text_box>();
        // tb1.brush().set_background(twf::Color::Light_blue);
        // tb1.brush().set_foreground(twf::Color::Yellow);
        auto& cpw = this->make_child<Click_paint_widget>();
        cpw.brush().set_foreground(twf::Color::Light_blue);
        cpw.enable_border();
        cpw.border().disable_east();
        cpw.border().disable_north_east();
        cpw.border().disable_south_east();
        cpw.border().disable_north();
        cpw.border().disable_north_west();
        // cpw.border().enable_corners();
        auto& tb2 = this->make_child<Text_box>();
        tb2.brush().set_foreground(twf::Color::Orange);
        tb2.border().disable_corners();
    }
};
