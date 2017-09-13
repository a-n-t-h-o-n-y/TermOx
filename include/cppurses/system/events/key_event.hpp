#ifndef SYSTEM_EVENTS_KEY_EVENT_HPP
#define SYSTEM_EVENTS_KEY_EVENT_HPP

#include "system/event.hpp"
#include "system/key.hpp"
#include <string>

namespace cppurses {

class Key_event : public Event {
   public:
    Key_event(Event::Type type, int key_code)
        : Event{type}, key_code_{key_code} {}
    Key key_code() const { return static_cast<Key>(key_code_); }
    std::string text() const {
        if (key_code_ < 32 || key_code_ > 126) {
            return std::string{};
        }
        return std::string{static_cast<char>(key_code_)};
    }

   private:
    int key_code_;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_KEY_EVENT_HPP
