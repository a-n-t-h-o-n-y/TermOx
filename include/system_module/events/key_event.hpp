#ifndef KEY_EVENT_HPP
#define KEY_EVENT_HPP

#include "../event.hpp"
#include "../key.hpp"

#include <string>

namespace mcurses {

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

}  // namespace mcurses
#endif  // KEY_EVENT_HPP
