#include <cppurses/system/events/resize_event.hpp>

#include <cstddef>

#include <cppurses/system/event_handler.hpp>
#include <cppurses/widget/area.hpp>

namespace cppurses {

Resize_event::Resize_event(Event_handler* receiver,
                           Area new_size,
                           Area old_size)
    : Event{Event::Resize, receiver},
      new_size_{new_size},
      old_size_{old_size} {}

bool Resize_event::send() const {
    // Getting seg faults on this line on resize. reciever_ is non null,
    // possibly deleted?
    return receiver_->resize_event(new_size_, old_size_);
}

bool Resize_event::filter_send(Event_handler* filter) const {
    return filter->resize_event_filter(receiver_, new_size_, old_size_);
}

}  // namespace cppurses
