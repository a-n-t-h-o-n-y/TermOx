#include <cppurses/system/events/keyboard.hpp>

#include <cppurses/system/focus.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/shortcuts.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

Keyboard::Event::Event(Input_event::Type type, Widget& receiver, Key key_code)
    : Input_event{type, receiver}, key_{key_code} {}

Keyboard::Press::Press(Widget& receiver, Key key_code)
    : Keyboard::Event{Event::KeyPress, receiver, key_code} {}

bool Keyboard::Press::send() const {
    if (Shortcuts::send_key(key_)) {
        return true;
    }
    if (key_ == Key::Tab && Focus::tab_press()) {
        return true;
    }
    if (key_ == Key::Back_tab && Focus::shift_tab_press()) {
        return true;
    }
    return receiver_.key_press_event(Keyboard::State{key_, key_to_char(key_)});
}

bool Keyboard::Press::filter_send(Widget& filter) const {
    return filter.key_press_event_filter(
        receiver_, Keyboard::State{key_, key_to_char(key_)});
}

Keyboard::Release::Release(Widget& receiver, Key key_code)
    : Keyboard::Event{Event::KeyRelease, receiver, key_code} {}

bool Keyboard::Release::send() const {
    return receiver_.key_release_event(
        Keyboard::State{key_, key_to_char(key_)});
}

bool Keyboard::Release::filter_send(Widget& filter) const {
    return filter.key_release_event_filter(
        receiver_, Keyboard::State{key_, key_to_char(key_)});
}
}  // namespace cppurses
