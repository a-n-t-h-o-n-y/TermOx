#ifndef CPPURSES_SYSTEM_EVENTS_MOUSE_EVENT_HPP
#define CPPURSES_SYSTEM_EVENTS_MOUSE_EVENT_HPP
#include <cstdint>

#include <cppurses/system/event.hpp>
#include <cppurses/system/events/input_event.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/system/mouse_data.hpp>
#include <cppurses/widget/point.hpp>

namespace cppurses {
class Widget;

class Mouse_event : public Input_event {
   public:
    Mouse_event(Event::Type type, Widget* receiver, const Mouse_data& mouse);

   protected:
    const Mouse_data mouse_;
};

class Mouse_press_event : public Mouse_event {
   public:
    Mouse_press_event(Widget* receiver, const Mouse_data& mouse);

    bool send() const override;
    bool filter_send(Widget* filter) const override;
};

class Mouse_release_event : public Mouse_event {
   public:
    Mouse_release_event(Widget* receiver, const Mouse_data& mouse);

    bool send() const override;
    bool filter_send(Widget* filter) const override;
};

class Mouse_double_click_event : public Mouse_event {
   public:
    Mouse_double_click_event(Widget* receiver, const Mouse_data& mouse);

    bool send() const override;
    bool filter_send(Widget* filter) const override;
};

class Mouse_wheel_event : public Mouse_event {
   public:
    Mouse_wheel_event(Widget* receiver, const Mouse_data& mouse);

    bool send() const override;
    bool filter_send(Widget* filter) const override;
};

class Mouse_move_event : public Mouse_event {
   public:
    Mouse_move_event(Widget* receiver, const Mouse_data& mouse);

    bool send() const override;
    bool filter_send(Widget* filter) const override;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENTS_MOUSE_EVENT_HPP
