#include "system/events/move_event.hpp"
#include "widget/widget.hpp"
#include <cstddef>

namespace cppurses {

Move_event::Move_event(Widget* receiver,
                       std::size_t new_x,
                       std::size_t new_y,
                       std::size_t old_x,
                       std::size_t old_y)
    : Event{Event::Move, receiver},
      new_x_{new_x},
      new_y_{new_y},
      old_x_{old_x},
      old_y_{old_y} {}

bool Move_event::send() const {
    return receiver_->move_event(new_x_, new_y_, old_x_, old_y_);
}

bool Move_event::filter_send(Widget* filter_widget) const {
    return filter_widget->move_event_filter(receiver_, new_x_, new_y_, old_x_,
                                            old_y_);
}

}  // namespace cppurses
