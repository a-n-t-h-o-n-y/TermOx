#ifndef CPPURSES_SYSTEM_EVENTS_MOUSE_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_MOUSE_EVENT_HPP
#include <cppurses/system/event.hpp>
#include <cppurses/system/events/input_event.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/system/mouse_data.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

class Mouse_event : public Input_event {
   public:
    Mouse_event(Event::Type type, Widget* receiver, const Mouse_data& mouse)
        : Input_event{type, receiver}, mouse_{mouse} {}

   protected:
    const Mouse_data mouse_;
};

class Mouse_press_event : public Mouse_event {
   public:
    Mouse_press_event(Widget* receiver, const Mouse_data& mouse)
        : Mouse_event{Event::MouseButtonPress, receiver, mouse} {}

    bool send() const override {
        Focus::mouse_press(receiver_);
        return receiver_->mouse_press_event(mouse_);
    }

    bool filter_send(Widget* filter) const override {
        return filter->mouse_press_event_filter(receiver_, mouse_);
    }
};

class Mouse_release_event : public Mouse_event {
   public:
    Mouse_release_event(Widget* receiver, const Mouse_data& mouse)
        : Mouse_event{Event::MouseButtonRelease, receiver, mouse} {}

    bool send() const override {
        return receiver_->mouse_release_event(mouse_);
    }

    bool filter_send(Widget* filter) const override {
        return filter->mouse_release_event_filter(receiver_, mouse_);
    }
};

class Mouse_double_click_event : public Mouse_event {
   public:
    Mouse_double_click_event(Widget* receiver, const Mouse_data& mouse)
        : Mouse_event{Event::MouseButtonDblClick, receiver, mouse} {}

    bool send() const override {
        return receiver_->mouse_double_click_event(mouse_);
    }

    bool filter_send(Widget* filter) const override {
        return filter->mouse_double_click_event_filter(receiver_, mouse_);
    }
};

class Mouse_wheel_event : public Mouse_event {
   public:
    Mouse_wheel_event(Widget* receiver, const Mouse_data& mouse)
        : Mouse_event{Event::MouseWheel, receiver, mouse} {}

    bool send() const override { return receiver_->mouse_wheel_event(mouse_); }

    bool filter_send(Widget* filter) const override {
        return filter->mouse_wheel_event_filter(receiver_, mouse_);
    }
};

class Mouse_move_event : public Mouse_event {
   public:
    Mouse_move_event(Widget* receiver, const Mouse_data& mouse)
        : Mouse_event{Event::MouseMove, receiver, mouse} {}

    bool send() const override {
        // TODO add mouse tracking and opt in bool to Widget so you don't get
        // spammed with mouse move events unless you want them.
        // if (receiver_->has_mouse_tracking()) {
        return receiver_->mouse_move_event(mouse_);
    }
    bool filter_send(Widget* filter) const override {
        return filter->mouse_move_event_filter(receiver_, mouse_);
    }
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_MOUSE_EVENT_HPP
