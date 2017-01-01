#ifndef ENABLE_EVENT_HPP
#define ENABLE_EVENT_HPP

#include "../event.hpp"

namespace twf {

class Enable_event : public Event {
   public:
    explicit Enable_event(bool enabled)
        : Event{Event::Type::EnabledChange}, enabled_{enabled} {}
    bool is_enabled() const { return enabled_; }

   private:
    bool enabled_;
};

}  // namespace twf
#endif  // ENABLE_EVENT_HPP
