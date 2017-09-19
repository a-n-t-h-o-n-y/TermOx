#ifndef SYSTEM_EVENTS_SHOW_EVENT_HPP
#define SYSTEM_EVENTS_SHOW_EVENT_HPP
#include "system/event.hpp"

namespace cppurses {
class Widget;

class Show_event : public Event {
   public:
    Show_event(Widget* receiver);
    bool send() const override;
    bool filter_send(Widget* filter_widget) const override;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_SHOW_EVENT_HPP
