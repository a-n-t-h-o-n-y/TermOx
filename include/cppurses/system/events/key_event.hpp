#ifndef SYSTEM_EVENTS_KEY_EVENT_HPP
#define SYSTEM_EVENTS_KEY_EVENT_HPP

#include "system/events/input_event.hpp"
#include "system/key.hpp"

namespace cppurses {
class Widget;

class Key_event : public Input_event {
   public:
    Key_event(Event::Type type, Widget* reciever, int key_code);

    void send() const override;

   private:
    const int key_code_;
};

// class key_press_event : public Key_event?
// class key_release_event : public Key_event?

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_KEY_EVENT_HPP
