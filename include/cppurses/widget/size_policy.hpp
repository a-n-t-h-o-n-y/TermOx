#ifndef WIDGET_SIZE_POLICY_HPP
#define WIDGET_SIZE_POLICY_HPP
#include <cstddef>
#include <limits>

namespace cppurses {

struct Size_policy {
    enum Type {
        Fixed,
        Minimum,
        Maximum,
        Preferred,
        Expanding,
        MinimumExpanding,
        Ignored
    };

    Type type{Type::Ignored};
    std::size_t stretch{1};
    std::size_t hint{0};
    std::size_t min{0};
    std::size_t max{std::numeric_limits<std::size_t>::max()};

    // bool height_for_width = false; // put in widget?
    // bool width_for_height = false;
};

}  // namespace cppurses
#endif  // WIDGET_SIZE_POLICY_HPP
