#include <cppurses/system/events/resize_event.hpp>

#include <cstddef>

#include <cppurses/system/event_handler.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

Resize_event::Resize_event(Event_handler* receiver,
                           Area new_size,
                           Area old_size)
    : Event{Event::Resize, receiver},
      new_size_{new_size},
      old_size_{old_size} {}

bool Resize_event::send() const {
    Widget* widg{static_cast<Widget*>(receiver_)};
    if (widg->outer_width() != new_size_.width ||
        widg->outer_height() != new_size_.height) {
        old_size_.width = widg->outer_width();
        old_size_.height = widg->outer_height();
        widg->outer_width_ = new_size_.width;
        widg->outer_height_ = new_size_.height;
        return receiver_->resize_event(new_size_, old_size_);
    }
    return true;
}

bool Resize_event::filter_send(Event_handler* filter) const {
    Widget* widg{static_cast<Widget*>(receiver_)};
    if (widg->outer_width() != new_size_.width ||
        widg->outer_height() != new_size_.height) {
        return filter->resize_event_filter(receiver_, new_size_, old_size_);
    }
    return true;
}

}  // namespace cppurses
