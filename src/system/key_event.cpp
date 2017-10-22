#include <cppurses/system/event_handler.hpp>
#include <cppurses/system/events/key_event.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/shortcuts.hpp>

namespace cppurses {

// class Key_event
Key_event::Key_event(Event::Type type, Event_handler* receiver, Key key_code)
    : Input_event{type, receiver}, key_code_{key_code} {}

// class Key_press_event
Key_press_event::Key_press_event(Event_handler* receiver, Key key_code)
    : Key_event{Event::KeyPress, receiver, key_code} {}

bool Key_press_event::send() const {
    if (!receiver_->enabled()) {
        return false;
    }
    // Hotkeys/Shortcuts
    if (Shortcuts::send_key(key_code_)) {
        return true;
    }
    if (key_code_ == Key::Tab && Focus::tab_press()) {
        return true;
    }
    return receiver_->key_press_event(key_code_, key_to_char(key_code_));
}

bool Key_press_event::filter_send(Event_handler* filter) const {
    return filter->key_press_event_filter(receiver_, key_code_,
                                          key_to_char(key_code_));
}

// class Key_release_event
Key_release_event::Key_release_event(Event_handler* receiver, Key key_code)
    : Key_event{Event::KeyRelease, receiver, key_code} {}

bool Key_release_event::send() const {
    if (receiver_->enabled()) {
        return receiver_->key_release_event(key_code_, key_to_char(key_code_));
    }
    return false;
}

bool Key_release_event::filter_send(Event_handler* filter) const {
    return filter->key_release_event_filter(receiver_, key_code_,
                                            key_to_char(key_code_));
}

}  // namespace cppurses
