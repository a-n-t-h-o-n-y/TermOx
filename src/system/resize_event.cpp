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

detail::Screen_mask build_resize_mask(const Widget* w,
                                      const Area& old_size,
                                      const Area& new_size) {
    detail::Screen_mask mask{*w};

    // Width
    if (new_size.width > old_size.width) {
        for (std::size_t y{w->y()}; y < (w->y() + new_size.height); ++y) {
            for (std::size_t x{w->x() + old_size.width};
                 x < (w->x() + new_size.width); ++x) {
                mask.at(x, y) = true;
            }
        }
    }

    // Height
    if (new_size.height > old_size.height) {
        for (std::size_t y{w->y() + old_size.height};
             y < (w->y() + new_size.height); ++y) {
            for (std::size_t x{w->x()}; x < (w->x() + new_size.width); ++x) {
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
bool Resize_event::send() const {
    receiver_->screen_state().optimize.resized = true;

    // Set old size from current receiver_et size.
    old_size_.width = receiver_->outer_width();
    old_size_.height = receiver_->outer_height();

    // Set receiver_et to new size.
    receiver_->outer_width_ = new_size_.width;
    receiver_->outer_height_ = new_size_.height;

    // Remove screen_state tiles if they are outside the new dimensions.
    auto iter = std::begin(receiver_->screen_state().tiles);
    auto end = std::end(receiver_->screen_state().tiles);
    while (iter != end) {
        Point p{iter->first};
        if (p.x >= receiver_->x() + receiver_->outer_width() ||
            p.y >= receiver_->y() + receiver_->outer_height()) {
            iter = receiver_->screen_state().tiles.erase(iter);
        } else {
            ++iter;
        }
    }

    // Create resize_mask for screen_state.optimize
    detail::Screen_mask mask{
        build_resize_mask(receiver_, old_size_, new_size_)};
    receiver_->screen_state().optimize.resize_mask = std::move(mask);

    return receiver_->resize_event(new_size_, old_size_);
}

bool Resize_event::filter_send(Widget* filter) const {
    if (receiver_->outer_width() != new_size_.width ||
        receiver_->outer_height() != new_size_.height) {
        return filter->resize_event_filter(receiver_, new_size_, old_size_);
    }
    return true;
}

}  // namespace cppurses
