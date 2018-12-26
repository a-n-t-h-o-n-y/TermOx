#include <cppurses/system/events/key_event.hpp>

#include <cppurses/system/focus.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/keyboard_data.hpp>
#include <cppurses/system/shortcuts.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

bool Key_press_event::send() const {
    if (Shortcuts::send_key(key_code_)) {
        return true;
    }
    if (key_code_ == Key::Tab && Focus::tab_press()) {
        return true;
    }
    if (key_code_ == Key::Back_tab && Focus::shift_tab_press()) {
        return true;
    }
    return receiver_.key_press_event(
        Keyboard_data{key_code_, key_to_char(key_code_)});
}

}  // namespace cppurses
