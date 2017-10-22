#ifndef SYSTEM_EVENTS_KEY_EVENT_HPP
#define SYSTEM_EVENTS_KEY_EVENT_HPP
#include <cppurses/system/event.hpp>
#include <cppurses/system/events/input_event.hpp>
#include <cppurses/system/key.hpp>

namespace cppurses {
class Event_handler;

class Key_event : public Input_event {
   public:
    Key_event(Event::Type type, Event_handler* receiver, Key key_code);

   protected:
    const Key key_code_;
};

class Key_press_event : public Key_event {
   public:
    Key_press_event(Event_handler* receiver, Key key_code);
    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

class Key_release_event : public Key_event {
   public:
    Key_release_event(Event_handler* receiver, Key key_code);
    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_KEY_EVENT_HPP
