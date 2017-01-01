#ifndef MAIN_WIDGET_HPP
#define MAIN_WIDGET_HPP

#include <twidgets.hpp>
using namespace twf;

class Main_widget : public Widget {
   public:
    Main_widget() {
        // this->close();
        this->set_focus_policy(Widget::Focus_policy::ClickFocus);
        this->set_cursor(true);
        this->enable_border();
        this->set_brush(
            Brush(background(Color::Green), foreground(Color::Orange)));
    }

    bool key_press_event(const Key_event& ke) override {
        if (ke.key_code() == Key::j) {
            this->close();  // triggers an infinite loop
            // System::exit();
            // System::post_event(this,
            // std::make_unique<Event>(Event::DeferredDelete));
        }
        return true;
    }
};

#endif  // MAIN_WIDGET_HPP
