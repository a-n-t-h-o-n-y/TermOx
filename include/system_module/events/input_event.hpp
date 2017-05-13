#ifndef SYSTEM_MODULE_EVENTS_INPUT_EVENT_HPP
#define SYSTEM_MODULE_EVENTS_INPUT_EVENT_HPP

#include "system_module/event.hpp"

namespace twf {

class Input_event : public Event {
   public:
    enum class KeyboardModifiers { None };

   protected:
    explicit Input_event(Event::Type type)
        : Event{type}, modifiers_{KeyboardModifiers::None} {}
    KeyboardModifiers modifiers_;
};

}  // namespace twf
#endif  // SYSTEM_MODULE_EVENTS_INPUT_EVENT_HPP
