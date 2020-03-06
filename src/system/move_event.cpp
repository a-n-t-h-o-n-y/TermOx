#include <cppurses/system/events/move_event.hpp>

#include <cppurses/painter/detail/screen_descriptor.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

auto Move_event::send() const -> bool
{
    if (receiver_.x() == new_position_.x and receiver_.y() == new_position_.y)
        return true;
    receiver_.screen_state().clear();
    auto const old_position = Point{receiver_.x(), receiver_.y()};
    receiver_.set_x(new_position_.x);
    receiver_.set_y(new_position_.y);
    return receiver_.move_event(new_position_, old_position);
}

auto Move_event::filter_send(Widget& filter) const -> bool
{
    if (receiver_.x() == new_position_.x and receiver_.y() == new_position_.y)
        return true;
    auto const old_position = Point{receiver_.x(), receiver_.y()};
    return filter.move_event_filter(receiver_, new_position_, old_position);
}

}  // namespace cppurses
