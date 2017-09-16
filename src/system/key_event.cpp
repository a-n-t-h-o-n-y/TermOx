#include "system/events/key_event.hpp"
#include "system/key.hpp"
#include <string>

namespace cppurses {
class Widget;

Key_event::Key_event(Event::Type type, Widget* reciever, int key_code)
    : Input_event{type, reciever}, key_code_{key_code} {}

Key Key_event::key_code() const {
    return static_cast<Key>(key_code_);
}

std::string Key_event::text() const {
    if (key_code_ < 32 || key_code_ > 126) {
        return std::string{};
    }
    return std::string{static_cast<char>(key_code_)};
}

void Key_event::send() const {
    reciever_->key_event(static_cast<Key>(key_code_), static_);
}

}  // namespace cppurses
