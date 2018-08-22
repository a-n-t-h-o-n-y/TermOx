#include <cppurses/painter/detail/find_empty_space.hpp>

#include <algorithm>
#include <fstream>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <vector>

#include <cppurses/painter/detail/screen_mask.hpp>
#include <cppurses/widget/widget.hpp>

namespace {
using namespace cppurses;

bool is_width_equal_to_parent(const std::unique_ptr<Widget>& w) {
    // parent should not be nullptr if you are here.
    return w->outer_width() == w->parent()->width();
}

bool is_height_equal_to_parent(const std::unique_ptr<Widget>& w) {
    // parent should not be nullptr if you are here.
    return w->outer_height() == w->parent()->height();
}

std::size_t sum_widths(std::size_t sum, const std::unique_ptr<Widget>& w) {
    return sum + w->outer_width();
}

std::size_t sum_heights(std::size_t sum, const std::unique_ptr<Widget>& w) {
    return sum + w->outer_height();
}

}  // namespace

namespace cppurses {
namespace detail {

/// Returns a screen mask for the Widget \p w, where a set bit means you have no
/// enabled child owning that point. Used to find where a Layout should paint
/// wallpaper tiles.
Screen_mask find_empty_space(const Widget& w) {
    // Optimize out calculation if no empty space. Vertical Layout
    const std::vector<std::unique_ptr<Widget>>& children{w.children.get()};
    std::size_t height_sum{std::accumulate(
        std::begin(children), std::end(children), std::size_t{0}, sum_heights)};
    bool widths_equal_parent{std::all_of(
        std::begin(children), std::end(children), is_width_equal_to_parent)};
    if (height_sum == w.height() && widths_equal_parent) {
        return Screen_mask{};
    }

    // Optimize out calculation if no empty space. Horizontal Layout
    std::size_t width_sum{std::accumulate(
        std::begin(children), std::end(children), std::size_t{0}, sum_widths)};
    bool heights_equal_parent{std::all_of(
        std::begin(children), std::end(children), is_height_equal_to_parent)};
    if (width_sum == w.width() && heights_equal_parent) {
        return Screen_mask{};
    }

    Screen_mask result(w);
    // for loop over each child, only perform action if it is enabled
    for (const std::unique_ptr<Widget>& child : children) {
        if (child->enabled()) {
            // Point child_offset{child->x(), child->y()};
            // Area child_area{child->outer_width(), child->outer_height()};
            for (std::size_t y{child->y()};
                 y < child->outer_height() + child->y(); ++y) {
                for (std::size_t x{child->x()};
                     x < child->outer_width() + child->x(); ++x) {
                    try {
                        result.at(x, y) = true;
                    } catch (std::out_of_range e) {
                        std::ofstream l{"find_empty_space_log.txt",
                                        std::ios::app};
                        static int i{0};
                        l << "out of bounds child " << i++ << std::endl;
                    }
                }
            }
        }
    }
    // invert it to get tiles not painted on.
    result.flip();
    return result;
}

}  // namespace detail
}  // namespace cppurses
