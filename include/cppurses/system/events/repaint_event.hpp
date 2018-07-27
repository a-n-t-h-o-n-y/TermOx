#ifndef CPPURSES_SYSTEM_REPAINT_EVENT_HPP
#define CPPURSES_SYSTEM_REPAINT_EVENT_HPP
#include <cppurses/system/event.hpp>

namespace cppurses {
class Event_handler;

class Repaint_event : public Event {
   public:
    explicit Repaint_event(Event_handler* receiver);
    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_REPAINT_EVENT_HPP
