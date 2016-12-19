#include <mcurses.hpp>

#include <string>

class Text_box : public mcurses::Widget {
   public:
    Text_box() {
        this->set_focus_policy(mcurses::Widget::Focus_policy::StrongFocus);
        this->set_cursor(true);
    }
    bool paint_event(const mcurses::Paint_event& event) override {
        this->erase_widget_screen();
        mcurses::Painter p{this};
        p.move(0, 0);
        p.put(contents_);
        return Widget::paint_event(event);
    }
    bool key_press_event(const mcurses::Key_event& event) override {
        contents_.append(event.text());
        this->update();  // every time you call update you are just putting a
                         // call to paint_event() in the queue.
        return Widget::key_press_event(event);
    }

   private:
    std::string contents_;
};

class Normal_widget : public mcurses::Widget {
   public:
    Normal_widget() {
        this->set_focus_policy(mcurses::Widget::Focus_policy::StrongFocus);
        this->set_cursor(true);
        this->enable_border();
    }
    bool paint_event(const mcurses::Paint_event& event) override {
        this->erase_widget_screen();
        mcurses::Painter p{this};
        p.move(1, 1);
        p.put("Normal Widget");
        p.move(this->geometry().width() / 2, this->geometry().height() / 2);
        p.put(text_);
        return Widget::paint_event(event);
        // return;
    }
    bool key_press_event(const mcurses::Key_event& event) override {
        text_ = event.text();
        this->update();
        return Widget::key_press_event(event);
    }

   private:
    std::string text_;  // change to glyph
};

class Click_paint_widget : public mcurses::Widget {
   public:
    Click_paint_widget() {
        this->set_cursor(false);
        this->enable_border();
    }

    bool paint_event(const mcurses::Paint_event& event) override {
        mcurses::Painter p{this};
        p.move(1, 1);
        p.put("Click Widget");
        return Widget::paint_event(event);
        // return;
    }

    bool mouse_press_event(const mcurses::Mouse_event& event) override {
        mcurses::Painter p{this};
        p.move(event.local_x(), event.local_y());
        p.put("X");
        return Widget::mouse_press_event(event);
    }
};

class Main_widget : public mcurses::Horizontal_layout {
   public:
    Main_widget() {
        this->make_child<Click_paint_widget>();
        this->make_child<Normal_widget>();
        this->make_child<Click_paint_widget>();
        this->make_child<Text_box>();
    }
};

int main() {
    mcurses::System system;

    Main_widget widg;

    system.set_head(&widg);

    return system.run();
}
