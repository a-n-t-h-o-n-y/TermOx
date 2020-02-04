#include <cppurses/system/events/mouse.hpp>

#include <cppurses/system/focus.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

auto Mouse::Press::send() const -> bool
{
    Focus::mouse_press(receiver_);
    return receiver_.mouse_press_event(data_);
}

auto Mouse::Press::filter_send(Widget& filter) const -> bool
{
    return filter.mouse_press_event_filter(receiver_, data_);
}

auto Mouse::Release::send() const -> bool
{
    return receiver_.mouse_release_event(data_);
}

auto Mouse::Release::filter_send(Widget& filter) const -> bool
{
    return filter.mouse_release_event_filter(receiver_, data_);
}

auto Mouse::Double_click::send() const -> bool
{
    return receiver_.mouse_double_click_event(data_);
}

auto Mouse::Double_click::filter_send(Widget& filter) const -> bool
{
    return filter.mouse_double_click_event_filter(receiver_, data_);
}

auto Mouse::Wheel::send() const -> bool
{
    return receiver_.mouse_wheel_event(data_);
}

auto Mouse::Wheel::filter_send(Widget& filter) const -> bool
{
    return filter.mouse_wheel_event_filter(receiver_, data_);
}

auto Mouse::Move::send() const -> bool
{
    return receiver_.mouse_move_event(data_);
}

auto Mouse::Move::filter_send(Widget& filter) const -> bool
{
    return filter.mouse_move_event_filter(receiver_, data_);
}
}  // namespace cppurses
