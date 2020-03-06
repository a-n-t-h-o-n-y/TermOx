#include <cppurses/system/events/resize_event.hpp>

#include <cstddef>
#include <iterator>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cppurses/painter/detail/screen_descriptor.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

auto Resize_event::send() const -> bool
{
    if (receiver_.outer_width() == new_area_.width and
        receiver_.outer_height() == new_area_.height)
        return true;

    const auto old_area =
        Area{receiver_.outer_width(), receiver_.outer_height()};

    // Set receiver_ to new size.
    receiver_.outer_width_  = new_area_.width;
    receiver_.outer_height_ = new_area_.height;

    // Remove screen_state tiles if they are outside the new dimensions.
    if (new_area_.width < old_area.width or
        new_area_.height < old_area.height) {
        auto iter      = std::begin(receiver_.screen_state());
        auto const end = std::end(receiver_.screen_state());
        while (iter != end) {
            auto const p = Point{iter->first};
            if (p.x >= receiver_.x() + receiver_.outer_width() or
                p.y >= receiver_.y() + receiver_.outer_height()) {
                iter = receiver_.screen_state().erase(iter);
            }
            else
                ++iter;
        }
    }
    return receiver_.resize_event(new_area_, old_area);
}

auto Resize_event::filter_send(Widget& filter) const -> bool
{
    if (receiver_.outer_width() == new_area_.width and
        receiver_.outer_height() == new_area_.height)
        return true;
    auto const old_area =
        Area{receiver_.outer_width(), receiver_.outer_height()};
    return filter.resize_event_filter(receiver_, new_area_, old_area);
}

}  // namespace cppurses
