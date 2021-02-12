#include <termox/widget/detail/border_offset.hpp>

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
    if (w.outer_width() == 2 && w.border.segments.west.enabled())
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
    if (w.outer_height() == 2 && w.border.segments.north.enabled())
        return true;
    return false;
}

auto Border_offset::west(Widget const& w) -> int
{
    return west_enabled(w) && !west_disqualified(w) ? 1 : 0;
}

auto Border_offset::east(Widget const& w) -> int
{
    return east_enabled(w) && !east_disqualified(w) ? 1 : 0;
}

auto Border_offset::north(Widget const& w) -> int
{
    return north_enabled(w) && !north_disqualified(w) ? 1 : 0;
}

auto Border_offset::south(Widget const& w) -> int
{
    return south_enabled(w) && !south_disqualified(w) ? 1 : 0;
}

auto Border_offset::west_enabled(Widget const& w) -> bool
{
    auto const& b = w.border.segments;
    return w.border.enabled() && (b.west.enabled() || b.north_west.enabled() ||
                                  b.south_west.enabled());
}

auto Border_offset::east_enabled(Widget const& w) -> bool
{
    auto const& b = w.border.segments;
    return w.border.enabled() && (b.east.enabled() || b.north_east.enabled() ||
                                  b.south_east.enabled());
}

auto Border_offset::north_enabled(Widget const& w) -> bool
{
    auto const& b = w.border.segments;
    return w.border.enabled() && (b.north.enabled() || b.north_east.enabled() ||
                                  b.north_west.enabled());
}

auto Border_offset::south_enabled(Widget const& w) -> bool
{
    auto const& b = w.border.segments;
    return w.border.enabled() && (b.south.enabled() || b.south_east.enabled() ||
                                  b.south_west.enabled());
}

}  // namespace ox::detail
