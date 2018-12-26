#ifndef CPPURSES_SYSTEM_EVENTS_KEY_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_KEY_EVENT_HPP
#include <cppurses/system/event.hpp>
#include <cppurses/system/events/input_event.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/keyboard_data.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

class Key_event : public Input_event {
   public:
    Key_event(Event::Type type, Widget& receiver, Key key_code)
        : Input_event{type, receiver}, key_code_{key_code} {}

   protected:
    const Key key_code_;
};

class Key_press_event : public Key_event {
   public:
    Key_press_event(Widget& receiver, Key key_code)
        : Key_event{Event::KeyPress, receiver, key_code} {}

    bool send() const override;

    bool filter_send(Widget& filter) const override {
        return filter.key_press_event_filter(
            receiver_, Keyboard_data{key_code_, key_to_char(key_code_)});
    }
};

class Key_release_event : public Key_event {
   public:
    Key_release_event(Widget& receiver, Key key_code)
        : Key_event{Event::KeyRelease, receiver, key_code} {}

    bool send() const override {
        return receiver_.key_release_event(
            Keyboard_data{key_code_, key_to_char(key_code_)});
    }

    bool filter_send(Widget& filter) const override {
        return filter.key_release_event_filter(
            receiver_, Keyboard_data{key_code_, key_to_char(key_code_)});
    }
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_KEY_EVENT_HPP
