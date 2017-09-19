#ifndef SYSTEM_EVENTS_FOCUS_EVENT_HPP
#define SYSTEM_EVENTS_FOCUS_EVENT_HPP
#include "system/event.hpp"

namespace cppurses {
class Widget;

// class Focus_event : public Event {
//    public:
//     explicit Focus_event(Event::Type type) : Event{type} {}
// };

class Focus_in_event : public Event {
   public:
    explicit Focus_in_event(Widget* receiver);
    bool send() const override;
    bool filter_send(Widget* filter_widget) const override;
};

class Focus_out_event : public Event {
   public:
    explicit Focus_out_event(Widget* receiver);
    bool send() const override;
    bool filter_send(Widget* filter_widget) const override;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_FOCUS_EVENT_HPP
