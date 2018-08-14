#include <cppurses/widget/detail/border_offset.hpp>

#include <cstddef>

#include <cppurses/widget/border.hpp>
#include <cppurses/widget/widget.hpp>

namespace {
using namespace cppurses;
bool west_border_disqualified(const Widget& w) {
    return w.outer_width() <= 1;
}
bool east_border_disqualified(const Widget& w) {
    return w.outer_width() <= 2;
}
bool north_border_disqualified(const Widget& w) {
    return w.outer_height() <= 1;
}
bool south_border_disqualified(const Widget& w) {
    return w.outer_height() <= 2;
}
}  // namespace

namespace cppurses {
namespace detail {

std::size_t Border_offset::west(const Widget& w) {
    const Border& b{w.border};
    if (b.enabled && !west_border_disqualified(w) &&
        (b.west_enabled || b.north_west_enabled || b.south_west_enabled)) {
        return 1;
    }
    return 0;
}

std::size_t Border_offset::east(const Widget& w) {
    const Border& b{w.border};
    if (b.enabled && !east_border_disqualified(w) &&
        (b.east_enabled || b.north_east_enabled || b.south_east_enabled)) {
        return 1;
    }
    return 0;
}

std::size_t Border_offset::north(const Widget& w) {
    const Border& b{w.border};
    if (b.enabled && !north_border_disqualified(w) &&
        (b.north_enabled || b.north_east_enabled || b.north_west_enabled)) {
        return 1;
    }
    return 0;
}

std::size_t Border_offset::south(const Widget& w) {
    const Border& b{w.border};
    if (b.enabled && !south_border_disqualified(w) &&
        (b.south_enabled || b.south_east_enabled || b.south_west_enabled)) {
        return 1;
    }
    return 0;
}

}  // namespace detail
}  // namespace cppurses
