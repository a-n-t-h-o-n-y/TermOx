#include <cppurses/system/events/move_event.hpp>

#include <cppurses/system/event_handler.hpp>
#include <cppurses/widget/point.hpp>

namespace cppurses {

Move_event::Move_event(Event_handler* receiver,
                       Point new_position,
                       Point old_position)
    : Event{Event::Move, receiver},
      new_position_{new_position},
      old_position_{old_position} {}

bool Move_event::send() const {
    return receiver_->move_event(new_position_, old_position_);
}

bool Move_event::filter_send(Event_handler* filter) const {
    return filter->move_event_filter(receiver_, new_position_, old_position_);
}

}  // namespace cppurses
