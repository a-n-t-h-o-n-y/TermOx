#include <cppurses/painter/detail/find_empty_space.hpp>

#include <fstream>
#include <memory>
#include <stdexcept>

#include <cppurses/painter/detail/screen_mask.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
namespace detail {

/// Returns a screen mask for the Widget \p w, where a set bit means you have no
/// enabled child owning that point. Used to find where a Layout should paint
/// wallpaper tiles.
Screen_mask find_empty_space(const Widget& w) {
    Screen_mask result(w);
    Point offset = result.offset();
    Area area = result.area();
    // for loop over each child, only perform action if it is enabled
    for (const std::unique_ptr<Widget>& child : w.children.get()) {
        if (child->enabled()) {
            Point child_offset{child->x(), child->y()};
            Area child_area{child->outer_width(), child->outer_height()};
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
