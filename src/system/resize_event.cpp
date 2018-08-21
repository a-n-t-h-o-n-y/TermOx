#include <cppurses/system/events/resize_event.hpp>

#include <cstddef>

#include <cppurses/painter/detail/screen_mask.hpp>
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
    // Removed below if because of enabling of children and optimizing out here
    // would cause the children not to be disabled again.
    // if (widg->outer_width() != new_size_.width ||
    //     widg->outer_height() != new_size_.height) {
    widg->screen_state().optimize.resized = true;
    // Create and set resize_mask in widg->screen_state()
    widg->screen_state().optimize.resize_mask = detail::Screen_mask(*widg);
    // Remove tiles in screen state that are outside of the new size.
    auto iter = std::begin(widg->screen_state().tiles);
    auto end = std::end(widg->screen_state().tiles);
    while (iter != end) {
        Point p{iter->first};
        if (p.x >= widg->x() + widg->outer_width() ||
            p.y >= widg->y() + widg->outer_height()) {
            iter = widg->screen_state().tiles.erase(iter);
        } else {
            ++iter;
        }
    }
    old_size_.width = widg->outer_width();
    old_size_.height = widg->outer_height();
    widg->outer_width_ = new_size_.width;
    widg->outer_height_ = new_size_.height;
    return receiver_->resize_event(new_size_, old_size_);
    // }
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
