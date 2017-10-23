#ifndef SYSTEM_EVENTS_INPUT_EVENT_HPP
#define SYSTEM_EVENTS_INPUT_EVENT_HPP
#include <cppurses/system/event.hpp>

namespace cppurses {
class Event_handler;

class Input_event : public Event {
   public:
    enum class KeyboardModifiers { None };

   protected:
    Input_event(Event::Type type, Event_handler* receiver)
        : Event{type, receiver}, modifiers_{KeyboardModifiers::None} {}
    KeyboardModifiers modifiers_;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_INPUT_EVENT_HPP
