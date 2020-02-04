#include <cppurses/system/events/resize_event.hpp>

#include <cstddef>
#include <iterator>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cppurses/painter/detail/screen_descriptor.hpp>
#include <cppurses/painter/detail/screen_mask.hpp>
#include <cppurses/painter/detail/screen_state.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace {
using namespace cppurses;
using namespace cppurses::detail;

auto build_resize_mask(const Widget& w,
                       Area const& old_size,
                       Area const& new_size) -> Screen_mask
{
    // w has been resized at this point, Areas are in outer_... form.
    Screen_mask mask{w, Screen_mask::Outer};
    // Width
    if (new_size.width > old_size.width) {
        auto const y_begin = w.y();
        auto const x_begin = w.x() + old_size.width;
        auto const y_end   = y_begin + new_size.height;
        auto const x_end   = w.x() + new_size.width;
        for (auto y = y_begin; y < y_end; ++y) {
            for (auto x = x_begin; x < x_end; ++x) {
                mask.at(x, y) = true;
            }
        }
    }
    // Height
    if (new_size.height > old_size.height) {
        auto const y_begin = w.y() + old_size.height;
        auto const x_begin = w.x();
        auto const y_end   = w.y() + new_size.height;
        auto const x_end   = w.x() + new_size.width;
        for (auto y = y_begin; y < y_end; ++y) {
            for (auto x = x_begin; x < x_end; ++x) {
                mask.at(x, y) = true;
            }
        }
    }
    return mask;
}
}  // namespace

namespace cppurses {

// Cannot optimize out this call if size is the same, layouts need to
// enable/disable their children.
auto Resize_event::send() const -> bool
{
    receiver_.screen_state().optimize.resized = true;
    const auto old_area =
        Area{receiver_.outer_width(), receiver_.outer_height()};

    // Set receiver_ to new size.
    receiver_.outer_width_  = new_area_.width;
    receiver_.outer_height_ = new_area_.height;

    // Remove screen_state tiles if they are outside the new dimensions.
    auto iter      = std::begin(receiver_.screen_state().tiles);
    auto const end = std::end(receiver_.screen_state().tiles);
    while (iter != end) {
        auto p = Point{iter->first};
        if (p.x >= receiver_.x() + receiver_.outer_width() or
            p.y >= receiver_.y() + receiver_.outer_height()) {
            iter = receiver_.screen_state().tiles.erase(iter);
        }
        else
            ++iter;
    }

    // Create resize_mask for screen_state.optimize
    detail::Screen_mask mask{build_resize_mask(receiver_, old_area, new_area_)};
    receiver_.screen_state().optimize.resize_mask = std::move(mask);

    return receiver_.resize_event(new_area_, old_area);
}

auto Resize_event::filter_send(Widget& filter) const -> bool
{
    if (receiver_.outer_width() != new_area_.width or
        receiver_.outer_height() != new_area_.height) {
        auto const old_area =
            Area{receiver_.outer_width(), receiver_.outer_height()};
        return filter.resize_event_filter(receiver_, new_area_, old_area);
    }
    return true;
}

}  // namespace cppurses
