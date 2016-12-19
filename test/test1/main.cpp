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
        // return;
    }
    bool key_press_event(const mcurses::Key_event& event) override {
        contents_.append(event.text());
        // move the cursor in some way even if the glyph is the same that is
        // already there? some better way of doing this? of inputting text
        // that can be reused in any type of widget? like
        // Widget::text_input(std::string& storage_to_append_to)?
        // which would move the cursor. This is an important factor, the fact
        // that the cursor is moving right now is just a result of how the paint
        // engine works, it is not a part of the text input responsibility yet.
        // you need to make cursor position important. You could possibly make
        // this an object so that it can be parameterized with different options
        // on how to take input text and how to store it in the location.
        // and you could have multiple objects in a single widget so you can 
        // store text in different places in different ways. Though you can't
        // really have two different places to take in input from a single widget
        // unless it is a composite widget, which you don't have to worry about
        // in the widget class. So maybe just a single object. and this object
        // is connected to the text input for the widget, it can store the input
        // in different places and with different parsing? or leave that up to
        // another object? like don't include different colors and attrs, that
        // can be changed by the brush object, that doesn't need to be a responsiblity
        // of text input, this should only focus on storing input, so if storing
        // in multiple ways the actual content of types letters, then do that.
        // but i'm not sure I am convinced, you type on a keyboard, you shouldn't 
        // have to parse that into something differnet, it seems like it is a 
        // one to one thing.
        // One place is control characters, but those are intercepted somewhere
        // else and do not even make it to the widget's text input.
        // you could have options for displaying? like does it scroll? is it locked
        // to a particular limit of chars? should that be decided by the storage device?
        // should characters be displayed with a proxy character instead like
        // for passwords? ****
        // This might be tightly connected to what the display or paint event
        // function does. Maybe a second object that paints text, you give it
        // a glyph string and it has options that tell it how to use the space
        // to display these glyphs, this has a lot to do with layout, not attributes
        // text input and display are big for curses type TUI, so provide some
        // tools.
        this->update();  // every time you call update you are just putting a
                         // call to paint_event() in the queue.
        return Widget::key_press_event(event);
        // return;
        // This event is delayed, meaning you have to wait for the loop to go
        // around again to see its effect. paint event is called next, but it
        // takes another user input to get it. check out the event loop.virtual 
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
