#ifndef SYSTEM_EVENTS_DELETE_EVENT_HPP
#define SYSTEM_EVENTS_DELETE_EVENT_HPP
#include <memory>

#include <cppurses/system/event.hpp>

namespace cppurses {
class Event_handler;
class Widget;

class Delete_event : public Event {
   public:
    Delete_event(Event_handler* receiver, std::unique_ptr<Widget> removed);

    bool send() const override;
    bool filter_send(Event_handler* filter) const override;

   private:
    mutable std::unique_ptr<Widget> removed_;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_DELETE_EVENT_HPP
