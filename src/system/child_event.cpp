#include <cppurses/system/events/child_event.hpp>

#include <cppurses/painter/detail/screen_state.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

auto Child_event::send() const -> bool
{
    receiver_.screen_state().optimize.child_event = true;
    return true;
}

auto Child_added_event::send() const -> bool
{
    Child_event::send();
    return receiver_.child_added_event(child_);
}

bool Child_added_event::filter_send(Widget& filter) const
{
    return filter.child_added_event_filter(receiver_, child_);
}

auto Child_removed_event::send() const -> bool
{
    Child_event::send();
    return receiver_.child_removed_event(child_);
}

auto Child_removed_event::filter_send(Widget& filter) const -> bool
{
    return filter.child_removed_event_filter(receiver_, child_);
}

auto Child_polished_event::send() const -> bool
{
    Child_event::send();
    return receiver_.child_polished_event(child_);
}

auto Child_polished_event::filter_send(Widget& filter) const -> bool
{
    return filter.child_polished_event_filter(receiver_, child_);
}

}  // namespace cppurses
