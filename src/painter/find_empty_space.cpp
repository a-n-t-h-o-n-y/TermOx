#include <cppurses/painter/detail/find_empty_space.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <numeric>
#include <vector>

#include <cppurses/painter/detail/screen_mask.hpp>
#include <cppurses/widget/children_data.hpp>
#include <cppurses/widget/widget.hpp>

namespace {
using namespace cppurses;

/// Return the sum total height of all widgets from [first, last).
template <typename Iter_t>
std::size_t heights(Iter_t first, Iter_t last) {
    auto sum_heights = [](auto sum, const auto& widget) {
        return widget->enabled() ? sum += widget->outer_height() : sum;
    };
    return std::accumulate(first, last, std::size_t{0}, sum_heights);
}

/// Return the sum total width of all widgets from [first, last).
template <typename Iter_t>
std::size_t widths(Iter_t first, Iter_t last) {
    auto sum_widths = [](auto sum, const auto& widget) {
        return widget->enabled() ? sum += widget->outer_width() : sum;
    };
    return std::accumulate(first, last, std::size_t{0}, sum_widths);
}

/// Check if each Widget from [first, last) has width equal to \p width.
template <typename Iter_t>
auto all_widths_equal_to(Iter_t first, Iter_t last, std::size_t width) {
    auto widths_equal = [width](const std::unique_ptr<Widget>& w) {
        return w->enabled() ? w->outer_width() == width : true;
    };
    return std::all_of(first, last, widths_equal);
}

/// Check if each Widget from [first,last) has height equal to \p height.
template <typename Iter_t>
auto all_heights_equal_to(Iter_t first, Iter_t last, std::size_t height) {
    auto heights_equal = [height](const std::unique_ptr<Widget>& w) {
        return w->enabled() ? w->outer_height() == height : true;
    };
    return std::all_of(first, last, heights_equal);
}

/// Check whether \p w's children completely cover \p w.
bool children_completely_cover(const Widget& w) {
    const auto first = std::begin(w.children.get());
    const auto last = std::end(w.children.get());
    // Vertical Layout
    if (heights(first, last) == w.height() &&
        all_widths_equal_to(first, last, w.width())) {
        return true;
    }
    // Horizontal Layout
    if (widths(first, last) == w.width() &&
        all_heights_equal_to(first, last, w.height())) {
        return true;
    }
    return false;
}

/// Return the lowest y coordinate of the widget in global coordinates.
std::size_t height_end(const std::unique_ptr<Widget>& w) {
    return w->y() + w->outer_height();
}

/// Return the right-most x coordinate of the widget in global coordinates.
std::size_t width_end(const std::unique_ptr<Widget>& w) {
    return w->x() + w->outer_width();
}

/// Set \p mask to true at each point where a child of \p w covers \p w.
void mark_covered_tiles(const Widget& w, detail::Screen_mask& mask) {
    for (const auto& child : w.children.get()) {
        if (!child->enabled()) {
            continue;
        }
        for (auto y = child->y(); y < height_end(child); ++y) {
            for (auto x = child->x(); x < width_end(child); ++x) {
                mask.at(x, y) = true;
            }
        }
    }
}

}  // namespace

namespace cppurses {
namespace detail {

//  Should not consider border space, since that will never be empty.
Screen_mask find_empty_space(const Widget& w) {
    if (children_completely_cover(w)) {
        return Screen_mask{};
    }
    auto mask = Screen_mask{w, Screen_mask::Inner};
    mark_covered_tiles(w, mask);
    mask.flip();
    return mask;
}

}  // namespace detail
}  // namespace cppurses
