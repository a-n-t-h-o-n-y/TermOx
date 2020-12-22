#include <termox/widget/detail/border_offset.hpp>

#include <cstddef>

#include <termox/widget/border.hpp>
#include <termox/widget/widget.hpp>

namespace ox::detail {

auto Border_offset::west_disqualified(Widget const& w) -> bool
{
    return w.outer_width() <= 1;
}

auto Border_offset::east_disqualified(Widget const& w) -> bool
{
    if (w.outer_width() <= 1)
        return true;
    if (w.outer_width() == 2 and w.border.segments.west.enabled())
        return true;
    return false;
}

auto Border_offset::north_disqualified(Widget const& w) -> bool
{
    return w.outer_height() <= 1;
}

auto Border_offset::south_disqualified(Widget const& w) -> bool
{
    if (w.outer_height() <= 1)
        return true;
    if (w.outer_height() == 2 and w.border.segments.north.enabled())
        return true;
    return false;
}

auto Border_offset::west(Widget const& w) -> std::size_t
{
    return west_enabled(w) and !west_disqualified(w) ? 1 : 0;
}

auto Border_offset::east(Widget const& w) -> std::size_t
{
    return east_enabled(w) and !east_disqualified(w) ? 1 : 0;
}

auto Border_offset::north(Widget const& w) -> std::size_t
{
    return north_enabled(w) and !north_disqualified(w) ? 1 : 0;
}

auto Border_offset::south(Widget const& w) -> std::size_t
{
    return south_enabled(w) and !south_disqualified(w) ? 1 : 0;
}

auto Border_offset::west_enabled(Widget const& w) -> bool
{
    auto const& b = w.border.segments;
    return w.border.enabled() and (b.west.enabled() or b.north_west.enabled() or
                                   b.south_west.enabled());
}

auto Border_offset::east_enabled(Widget const& w) -> bool
{
    auto const& b = w.border.segments;
    return w.border.enabled() and (b.east.enabled() or b.north_east.enabled() or
                                   b.south_east.enabled());
}

auto Border_offset::north_enabled(Widget const& w) -> bool
{
    auto const& b = w.border.segments;
    return w.border.enabled() and
           (b.north.enabled() or b.north_east.enabled() or
            b.north_west.enabled());
}

auto Border_offset::south_enabled(Widget const& w) -> bool
{
    auto const& b = w.border.segments;
    return w.border.enabled() and
           (b.south.enabled() or b.south_east.enabled() or
            b.south_west.enabled());
}

}  // namespace ox::detail
