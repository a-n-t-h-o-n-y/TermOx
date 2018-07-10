#ifndef SYSTEM_EVENTS_PAINT_EVENT_HPP
#define SYSTEM_EVENTS_PAINT_EVENT_HPP
#include <cppurses/system/event.hpp>

#include <cppurses/painter/painter.hpp>

namespace cppurses {
class Event_handler;

class Paint_event : public Event {
   public:
    explicit Paint_event(Event_handler* receiver,
                         bool repaint_background = false);
    bool send() const override;
    bool filter_send(Event_handler* filter) const override;

   private:
    mutable Painter painter_;
    bool repaint_background_;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_PAINT_EVENT_HPP
