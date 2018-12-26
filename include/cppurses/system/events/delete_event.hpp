#ifndef CPPURSES_SYSTEM_EVENTS_DELETE_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_DELETE_EVENT_HPP
#include <memory>
#include <utility>

#include <cppurses/system/event.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

/// Send to a Widget just before it is to be destroyed.
/** The Widget will have already been removed from its parent Widget by the time
 *  it receives this event. All children of the receiver will receive this Event
 *  type as well. */
class Delete_event : public Event {
   public:
    Delete_event(Widget& receiver, std::unique_ptr<Widget> removed)
        : Event{Event::Delete, receiver}, removed_{std::move(removed)} {}

    bool send() const override;

    bool filter_send(Widget& filter) const override {
        return filter.delete_event_filter(receiver_);
    }

   private:
    mutable std::unique_ptr<Widget> removed_;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_DELETE_EVENT_HPP
