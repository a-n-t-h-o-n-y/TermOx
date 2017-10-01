#include "system/events/resize_event.hpp"
#include "system/event_handler.hpp"
#include <cstddef>

namespace cppurses {

Resize_event::Resize_event(Event_handler* receiver,
                           std::size_t new_width,
                           std::size_t new_height,
                           std::size_t old_width,
                           std::size_t old_height)
    : Event{Event::Resize, receiver},
      new_width_{new_width},
      new_height_{new_height},
      old_width_{old_width},
      old_height_{old_height} {}

bool Resize_event::send() const {
    return receiver_->resize_event(new_width_, new_height_, old_width_,
                                   old_height_);
}

bool Resize_event::filter_send(Event_handler* filter) const {
    return filter->resize_event_filter(
        receiver_, new_width_, new_height_, old_width_, old_height_);
}

}  // namespace cppurses
