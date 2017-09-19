#ifndef SYSTEM_EVENTS_MOUSE_EVENT_HPP
#define SYSTEM_EVENTS_MOUSE_EVENT_HPP
#include "system/events/input_event.hpp"
#include <cstddef>
#include <cstdint>

namespace cppurses {
class Event_handler;

enum class Mouse_button { None, Left, Middle, Right, ScrollUp, ScrollDown };

class Mouse_event : public Input_event {
   public:
    Mouse_event(Event::Type type,
                Event_handler* receiver,
                Mouse_button button,
                std::size_t glob_x,
                std::size_t glob_y,
                std::size_t local_x,
                std::size_t local_y,
                std::uint8_t device_id);

   protected:
    const Mouse_button button_;
    const std::size_t glob_x_;
    const std::size_t glob_y_;
    const std::size_t local_x_;
    const std::size_t local_y_;
    const std::uint8_t device_id_;
};

class Mouse_press_event : public Mouse_event {
   public:
    Mouse_press_event(Event_handler* receiver,
                      Mouse_button button,
                      std::size_t glob_x,
                      std::size_t glob_y,
                      std::size_t local_x,
                      std::size_t local_y,
                      std::uint8_t device_id);

    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

class Mouse_release_event : public Mouse_event {
   public:
    Mouse_release_event(Event_handler* receiver,
                        Mouse_button button,
                        std::size_t glob_x,
                        std::size_t glob_y,
                        std::size_t local_x,
                        std::size_t local_y,
                        std::uint8_t device_id);

    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

class Mouse_double_click_event : public Mouse_event {
   public:
    Mouse_double_click_event(Event_handler* receiver,
                             Mouse_button button,
                             std::size_t glob_x,
                             std::size_t glob_y,
                             std::size_t local_x,
                             std::size_t local_y,
                             std::uint8_t device_id);

    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

class Mouse_wheel_event : public Mouse_event {
   public:
    Mouse_wheel_event(Event_handler* receiver,
                      Mouse_button button,
                      std::size_t glob_x,
                      std::size_t glob_y,
                      std::size_t local_x,
                      std::size_t local_y,
                      std::uint8_t device_id);

    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

class Mouse_move_event : public Mouse_event {
   public:
    Mouse_move_event(Event_handler* receiver,
                     Mouse_button button,
                     std::size_t glob_x,
                     std::size_t glob_y,
                     std::size_t local_x,
                     std::size_t local_y,
                     std::uint8_t device_id);

    bool send() const override;
    bool filter_send(Event_handler* filter) const override;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_MOUSE_EVENT_HPP
