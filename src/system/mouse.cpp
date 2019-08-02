#include <cppurses/system/events/mouse.hpp>

#include <cppurses/system/focus.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

Mouse::Event::Event(Input_event::Type type, Widget& receiver, const State& data)
    : Input_event{type, receiver}, data_{data} {}

Mouse::Press::Press(Widget& receiver, const State& data)
    : Mouse::Event{Input_event::MouseButtonPress, receiver, data} {}

bool Mouse::Press::send() const {
    Focus::mouse_press(receiver_);
    return receiver_.mouse_press_event(data_);
}

bool Mouse::Press::filter_send(Widget& filter) const {
    return filter.mouse_press_event_filter(receiver_, data_);
}

Mouse::Release::Release(Widget& receiver, const State& data)
    : Mouse::Event{Input_event::MouseButtonRelease, receiver, data} {}

bool Mouse::Release::send() const {
    return receiver_.mouse_release_event(data_);
}

bool Mouse::Release::filter_send(Widget& filter) const {
    return filter.mouse_release_event_filter(receiver_, data_);
}

Mouse::Double_click::Double_click(Widget& receiver, const State& data)
    : Mouse::Event{Input_event::MouseButtonDblClick, receiver, data} {}

bool Mouse::Double_click::send() const {
    return receiver_.mouse_double_click_event(data_);
}

bool Mouse::Double_click::filter_send(Widget& filter) const {
    return filter.mouse_double_click_event_filter(receiver_, data_);
}

Mouse::Wheel::Wheel(Widget& receiver, const State& data)
    : Mouse::Event{Input_event::MouseWheel, receiver, data} {}

bool Mouse::Wheel::send() const {
    return receiver_.mouse_wheel_event(data_);
}

bool Mouse::Wheel::filter_send(Widget& filter) const {
    return filter.mouse_wheel_event_filter(receiver_, data_);
}

Mouse::Move::Move(Widget& receiver, const State& data)
    : Mouse::Event{Input_event::MouseMove, receiver, data} {}

bool Mouse::Move::send() const {
    return receiver_.mouse_move_event(data_);
}

bool Mouse::Move::filter_send(Widget& filter) const {
    return filter.mouse_move_event_filter(receiver_, data_);
}
}  // namespace cppurses
