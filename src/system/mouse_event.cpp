#include <cppurses/system/events/mouse_event.hpp>

#include <cstdint>

#include <cppurses/system/event.hpp>
#include <cppurses/system/events/input_event.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/system/mouse_data.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

// MOUSE_EVENT - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Mouse_event::Mouse_event(Event::Type type,
                         Widget* receiver,
                         const Mouse_data& mouse)
    : Input_event{type, receiver}, mouse_{mouse} {}

// MOUSE_PRESS_EVENT - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Mouse_press_event::Mouse_press_event(Widget* receiver, const Mouse_data& mouse)
    : Mouse_event{Event::MouseButtonPress, receiver, mouse} {}

bool Mouse_press_event::send() const {
    Focus::mouse_press(receiver_);
    return receiver_->mouse_press_event(mouse_);
}

bool Mouse_press_event::filter_send(Widget* filter) const {
    return filter->mouse_press_event_filter(receiver_, mouse_);
}

// MOUSE_RELEASE_EVENT - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Mouse_release_event::Mouse_release_event(Widget* receiver,
                                         const Mouse_data& mouse)
    : Mouse_event{Event::MouseButtonRelease, receiver, mouse} {}

bool Mouse_release_event::send() const {
    return receiver_->mouse_release_event(mouse_);
}

bool Mouse_release_event::filter_send(Widget* filter) const {
    return filter->mouse_release_event_filter(receiver_, mouse_);
}

// MOUSE_DOUBLE_CLICK_EVENT - - - - - - - - - - - - - - - - - - - - - - - - - -
Mouse_double_click_event::Mouse_double_click_event(Widget* receiver,
                                                   const Mouse_data& mouse)
    : Mouse_event{Event::MouseButtonDblClick, receiver, mouse} {}

bool Mouse_double_click_event::send() const {
    return receiver_->mouse_double_click_event(mouse_);
}

bool Mouse_double_click_event::filter_send(Widget* filter) const {
    return filter->mouse_double_click_event_filter(receiver_, mouse_);
}

// MOUSE_WHEEL_EVENT - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Mouse_wheel_event::Mouse_wheel_event(Widget* receiver, const Mouse_data& mouse)
    : Mouse_event{Event::MouseWheel, receiver, mouse} {}

bool Mouse_wheel_event::send() const {
    return receiver_->mouse_wheel_event(mouse_);
}

bool Mouse_wheel_event::filter_send(Widget* filter) const {
    return filter->mouse_wheel_event_filter(receiver_, mouse_);
}

// MOUSE_MOVE_EVENT - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Mouse_move_event::Mouse_move_event(Widget* receiver, const Mouse_data& mouse)
    : Mouse_event{Event::MouseMove, receiver, mouse} {}

bool Mouse_move_event::send() const {
    // TODO add mouse tracking and opt in bool to Widget so you don't get
    // spammed with mouse move events unless you want them.
    // if (receiver_->has_mouse_tracking()) {
    return receiver_->mouse_move_event(mouse_);
}

bool Mouse_move_event::filter_send(Widget* filter) const {
    return filter->mouse_move_event_filter(receiver_, mouse_);
}

}  // namespace cppurses
