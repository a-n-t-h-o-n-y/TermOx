#include <cppurses/system/events/key.hpp>

#include <cstdint>

#include <cppurses/system/focus.hpp>
#include <cppurses/system/shortcuts.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

Key::Event::Event(Input_event::Type type, Widget& receiver, Code key)
    : Input_event{type, receiver}, key_{key} {}

Key::Press::Press(Widget& receiver, Code key)
    : Key::Event{Event::KeyPress, receiver, key} {}

bool Key::Press::send() const {
    if (Shortcuts::send_key(key_)) {
        return true;
    }
    if (key_ == Code::Tab && Focus::tab_press()) {
        return true;
    }
    if (key_ == Code::Back_tab && Focus::shift_tab_press()) {
        return true;
    }
    return receiver_.key_press_event(Key::State{key_, key_to_char(key_)});
}

bool Key::Press::filter_send(Widget& filter) const {
    return filter.key_press_event_filter(receiver_,
                                         Key::State{key_, key_to_char(key_)});
}

Key::Release::Release(Widget& receiver, Code key)
    : Key::Event{Event::KeyRelease, receiver, key} {}

bool Key::Release::send() const {
    return receiver_.key_release_event(Key::State{key_, key_to_char(key_)});
}

bool Key::Release::filter_send(Widget& filter) const {
    return filter.key_release_event_filter(receiver_,
                                           Key::State{key_, key_to_char(key_)});
}

char key_to_char(Key::Code key) {
    const auto alpha_low = std::int16_t{32};
    const auto alpha_high = std::int16_t{126};
    const auto value = static_cast<std::int16_t>(key);
    if (value < alpha_low || value > alpha_high) {
        return '\0';
    }
    return static_cast<char>(value);
}
}  // namespace cppurses
