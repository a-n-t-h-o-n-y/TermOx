#include "system/events/key_event.hpp"
#include "system/key.hpp"
#include "widget/widget.hpp"

namespace cppurses {

// class Key_event
Key_event::Key_event(Event::Type type, Widget* receiver, Key key_code)
    : Input_event{type, receiver}, key_code_{key_code} {}

// class Key_press_event
Key_press_event::Key_press_event(Widget* receiver, Key key_code)
    : Key_event{Event::KeyPress, receiver, key_code} {}

bool Key_press_event::send() const {
    if (receiver_->visible() && receiver_->enabled()) {
        return receiver_->key_press_event(key_code_, key_to_char(key_code_));
    }
    return false;
}

bool Key_press_event::filter_send(Widget* filter_widget) const {
    return filter_widget->key_press_event_filter(receiver_, key_code_,
                                                 key_to_char(key_code_));
}

// class Key_release_event
Key_release_event::Key_release_event(Widget* receiver, Key key_code)
    : Key_event{Event::KeyRelease, receiver, key_code} {}

bool Key_release_event::send() const {
    if (receiver_->visible() && receiver_->enabled()) {
        return receiver_->key_release_event(key_code_, key_to_char(key_code_));
    }
    return false;
}

bool Key_release_event::filter_send(Widget* filter_widget) const {
    return filter_widget->key_release_event_filter(receiver_, key_code_,
                                                   key_to_char(key_code_));
}

}  // namespace cppurses
