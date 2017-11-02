#ifndef SYSTEM_EVENTS_MOUSE_EVENT_HPP
#define SYSTEM_EVENTS_MOUSE_EVENT_HPP
#include <cppurses/system/event.hpp>
#include <cppurses/system/events/input_event.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/widget/point.hpp>

#include <cstddef>
#include <cstdint>

namespace cppurses {
class Event_handler;

class Mouse_event : public Input_event {
   public:
    Mouse_event(Event::Type type,
                Event_handler* receiver,
                Mouse_button button,
                Point global,
                Point local,
                std::uint8_t device_id);

   protected:
    const Mouse_button button_;
    const Point global_;
    const Point local_;
    const std::uint8_t device_id_;
};

class Mouse_press_event : public Mouse_event {
   public:
    Mouse_press_event(Event_handler* receiver,
                      Mouse_button button,
                      Point global,
                      Point local,
                      std::uint8_t device_id);

    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

class Mouse_release_event : public Mouse_event {
   public:
    Mouse_release_event(Event_handler* receiver,
                        Mouse_button button,
                        Point global,
                        Point local,
                        std::uint8_t device_id);

    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

class Mouse_double_click_event : public Mouse_event {
   public:
    Mouse_double_click_event(Event_handler* receiver,
                             Mouse_button button,
                             Point global,
                             Point local,
                             std::uint8_t device_id);

    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

class Mouse_wheel_event : public Mouse_event {
   public:
    Mouse_wheel_event(Event_handler* receiver,
                      Mouse_button button,
                      Point global,
                      Point local,
                      std::uint8_t device_id);

    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

class Mouse_move_event : public Mouse_event {
   public:
    Mouse_move_event(Event_handler* receiver,
                     Mouse_button button,
                     Point global,
                     Point local,
                     std::uint8_t device_id);

    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_MOUSE_EVENT_HPP
