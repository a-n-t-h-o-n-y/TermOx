#ifndef SYSTEM_EVENTS_ON_TREE_EVENT_HPP
#define SYSTEM_EVENTS_ON_TREE_EVENT_HPP
#include <cppurses/system/event.hpp>

namespace cppurses {
class Event_handler;

class On_tree_event : public Event {
   public:
    On_tree_event(Event_handler* receiver, bool on_tree);
    bool send() const override;
    bool filter_send(Event_handler* filter) const override;

   private:
    const bool on_tree_;
};

}  // namespace cppurses

#endif  // SYSTEM_EVENTS_ON_TREE_EVENT_HPP
