#ifndef SYSTEM_EVENTS_ENABLE_EVENT_HPP
#define SYSTEM_EVENTS_ENABLE_EVENT_HPP

#include "system/event.hpp"

namespace cppurses {

class Enable_event : public Event {
   public:
    explicit Enable_event(bool enabled)
        : Event{Event::EnabledChange}, enabled_{enabled} {}
    bool is_enabled() const { return enabled_; }

   private:
    bool enabled_;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_ENABLE_EVENT_HPP
