#include <twidgets.hpp>
using namespace twf;

class Text_box : public twf::Widget {
   public:
    Text_box() {
        this->set_focus_policy(twf::Focus_policy::Strong);
        this->enable_cursor();
        this->enable_border();
        this->size_policy().horizontal_stretch = 5;
    }
    bool paint_event(const twf::Paint_event& event) override {
        twf::Painter p{this};
        p.put(contents_);
        return Widget::paint_event(event);
    }
    bool key_press_event(const twf::Key_event& event) override {
        contents_.append(event.text());
        this->update();  // every time you call update you are just putting a
                         // call to paint_event() in the queue.
        return Widget::key_press_event(event);
    }

   private:
    std::string contents_;
};

class Textbox2 : public Widget {
   public:
    Textbox2() {
        this->set_focus_policy(twf::Focus_policy::Strong);
        this->enable_cursor();
        this->enable_border();
        this->brush().set_background(Color::Green);
        this->brush().set_foreground(Color::White);
    }

    bool key_press_event(const Key_event& event) override {
        gs_.append(event.text(), background(Color::Red),
                   foreground(Color::Blue));
        this->update();
        return Widget::key_press_event(event);
    }

    bool paint_event(const Paint_event& event) override {
        Painter p{this};
        p.put(gs_);
        return Widget::paint_event(event);
    }

   private:
    Glyph_string gs_;
};

class My_widg : public Horizontal_layout {
   public:
    My_widg() {
        this->make_child<Textbox2>();
        this->make_child<Textbox2>();
        this->make_child<Text_box>();
    }
};

int main() {
    System system;

    My_widg main_widg;

    system.set_head(&main_widg);

    return system.run();
}
