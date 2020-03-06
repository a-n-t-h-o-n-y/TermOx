#include <cppurses/painter/detail/find_empty_space.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <numeric>
#include <vector>

#include <cppurses/painter/detail/screen_mask.hpp>
#include <cppurses/widget/widget.hpp>

namespace {
using namespace cppurses;

/// Return the sum total height of all widgets from [first, last).
template <typename Iter_t>
auto heights(Iter_t begin, Iter_t end) -> std::size_t
{
    auto const sum_heights = [](auto sum, auto const& widget) {
        return widget.enabled() ? sum += widget.outer_height() : sum;
    };
    return std::accumulate(begin, end, 0uL, sum_heights);
}

/// Return the sum total width of all widgets from [first, last).
template <typename Iter_t>
auto widths(Iter_t first, Iter_t last) -> std::size_t
{
    auto const sum_widths = [](auto sum, auto const& widget) {
        return widget.enabled() ? sum += widget.outer_width() : sum;
    };
    return std::accumulate(first, last, 0uL, sum_widths);
}

/// Check if each Widget from [first, last) has width equal to \p width.
template <typename Iter_t>
auto all_widths_equal_to(Iter_t first, Iter_t last, std::size_t width)
{
    auto const widths_equal = [width](Widget const& w) {
        return w.enabled() ? w.outer_width() == width : true;
    };
    return std::all_of(first, last, widths_equal);
}

/// Check if each Widget from [first,last) has height equal to \p height.
template <typename Iter_t>
auto all_heights_equal_to(Iter_t first, Iter_t last, std::size_t height)
{
    auto const heights_equal = [height](Widget const& w) {
        return w.enabled() ? w.outer_height() == height : true;
    };
    return std::all_of(first, last, heights_equal);
}

/// Check whether \p w's children completely cover \p w.
auto children_completely_cover(Widget const& w) -> bool
{
    auto const begin = std::cbegin(w.get_children());
    auto const end   = std::cend(w.get_children());
    // Vertical Layout
    if (heights(begin, end) == w.height() and
        all_widths_equal_to(begin, end, w.width())) {
        return true;
    }
    // Horizontal Layout
    if (widths(begin, end) == w.width() and
        all_heights_equal_to(begin, end, w.height())) {
        return true;
    }
    return false;
}

/// Return the lowest y coordinate of the widget in global coordinates.
auto height_end(Widget const& w) -> std::size_t
{
    return w.y() + w.outer_height();
}

/// Return the right-most x coordinate of the widget in global coordinates.
auto width_end(Widget const& w) -> std::size_t
{
    return w.x() + w.outer_width();
}

/// Set \p mask to true at each point where a child of \p w covers \p w.
void mark_covered_tiles(Widget const& w, detail::Screen_mask& mask)
{
    for (auto const& child : w.get_children()) {
        if (!child.enabled())
            continue;
        for (auto y = child.y(); y < height_end(child); ++y) {
            for (auto x = child.x(); x < width_end(child); ++x) {
                mask.at(x, y) = true;
            }
        }
    }
}

}  // namespace

namespace cppurses {
namespace detail {

//  Should not consider border space, since that will never be empty.
auto find_empty_space(Widget const& w) -> Screen_mask
{
    if (children_completely_cover(w))
        return {};
    auto mask = Screen_mask{w, Screen_mask::Inner};
    mark_covered_tiles(w, mask);
    mask.flip();
    return mask;
}

}  // namespace detail
}  // namespace cppurses
