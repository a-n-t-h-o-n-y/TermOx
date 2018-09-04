#ifndef CPPURSES_SYSTEM_EVENTS_DELETE_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_DELETE_EVENT_HPP
#include <memory>

#include <cppurses/system/event.hpp>

namespace cppurses {
class Event_handler;
class Widget;

/// Send to a Widget just before it is to be destroyed.
/** The Widget will have already be removed from its parent Widget by the time
 *  it receives this event. All children of the receiver will receive this Event
 *  as well. */
class Delete_event : public Event {
   public:
    Delete_event(Event_handler* receiver, std::unique_ptr<Widget> removed);

    bool send() const override;
    bool filter_send(Event_handler* filter) const override;

   private:
    mutable std::unique_ptr<Widget> removed_;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_DELETE_EVENT_HPP
