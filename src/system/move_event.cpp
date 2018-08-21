#include <cppurses/system/events/move_event.hpp>

#include <cppurses/painter/detail/screen_mask.hpp>
#include <cppurses/system/event_handler.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

Move_event::Move_event(Event_handler* receiver,
                       Point new_position,
                       Point old_position)
    : Event{Event::Move, receiver},
      new_position_{new_position},
      old_position_{old_position} {}

bool Move_event::send() const {
    Widget* widg{static_cast<Widget*>(receiver_)};
    if (widg->x() != new_position_.x || widg->y() != new_position_.y) {
        widg->screen_state().optimize.moved = true;
        // Create and set move_mask in widg->screen_state()
        widg->screen_state().optimize.move_mask = detail::Screen_mask(*widg);
        old_position_.x = widg->x();
        old_position_.y = widg->y();
        widg->set_x(new_position_.x);
        widg->set_y(new_position_.y);
        return receiver_->move_event(new_position_, old_position_);
    }
    return true;
}

bool Move_event::filter_send(Event_handler* filter) const {
    Widget* widg{static_cast<Widget*>(receiver_)};
    if (widg->x() != new_position_.x || widg->y() != new_position_.y) {
        return filter->move_event_filter(receiver_, new_position_,
                                         old_position_);
    }
    return true;
}

}  // namespace cppurses
