#ifndef SYSTEM_MODULE_EVENTS_ENABLE_EVENT_HPP
#define SYSTEM_MODULE_EVENTS_ENABLE_EVENT_HPP

#include "system_module/event.hpp"

namespace twf {

class Enable_event : public Event {
   public:
    explicit Enable_event(bool enabled)
        : Event{Event::EnabledChange}, enabled_{enabled} {}
    bool is_enabled() const { return enabled_; }

   private:
    bool enabled_;
};

}  // namespace twf
#endif  // SYSTEM_MODULE_EVENTS_ENABLE_EVENT_HPP
