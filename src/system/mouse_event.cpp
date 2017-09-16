#include "system/events/mouse_event.hpp"
#include "widget/widget.hpp"
#include <cstddef>
#include <cstdint>

namespace cppurses {
class Widget;

Mouse_event::Mouse_event(Event::Type type,
                         Widget* reciever,
                         Mouse_button button,
                         std::size_t glob_x,
                         std::size_t glob_y,
                         std::size_t local_x,
                         std::size_t local_y,
                         std::uint8_t device_id)
    : Input_event{type, reciever},
      button_{button},
      glob_x_{glob_x},
      glob_y_{glob_y},
      local_x_{local_x},
      local_y_{local_y},
      device_id_{device_id} {}

Mouse_press_event::Mouse_press_event(Widget* reciever,
                                     Mouse_button button,
                                     std::size_t glob_x,
                                     std::size_t glob_y,
                                     std::size_t local_x,
                                     std::size_t local_y,
                                     std::uint8_t device_id)
    : Mouse_event{Event::MouseButtonPress,
                  reciever,
                  button,
                  glob_x,
                  glob_y,
                  local_x,
                  local_y,
                  device_id} {}

void Mouse_press_event::send() const {
    reciever_->mouse_press_event(button_, glob_x_, glob_y_, local_x_, local_y_,
                                 device_id_);
}

Mouse_release_event::Mouse_release_event(Widget* reciever,
                                         Mouse_button button,
                                         std::size_t glob_x,
                                         std::size_t glob_y,
                                         std::size_t local_x,
                                         std::size_t local_y,
                                         std::uint8_t device_id)
    : Mouse_event{Event::MouseButtonRelease,
                  reciever,
                  button,
                  glob_x,
                  glob_y,
                  local_x,
                  local_y,
                  device_id} {}

void Mouse_release_event::send() const {
    reciever_->mouse_release_event(button_, glob_x_, glob_y_, local_x_,
                                   local_y_, device_id_);
}

Mouse_double_click_event::Mouse_double_click_event(Widget* reciever,
                                                   Mouse_button button,
                                                   std::size_t glob_x,
                                                   std::size_t glob_y,
                                                   std::size_t local_x,
                                                   std::size_t local_y,
                                                   std::uint8_t device_id)
    : Mouse_event{Event::MouseButtonDblClick,
                  reciever,
                  button,
                  glob_x,
                  glob_y,
                  local_x,
                  local_y,
                  device_id} {}

void Mouse_double_click_event::send() const {
    reciever_->mouse_double_click_event(button_, glob_x_, glob_y_, local_x_,
                                        local_y_, device_id_);
}

Mouse_wheel_event::Mouse_wheel_event(Widget* reciever,
                                     Mouse_button button,
                                     std::size_t glob_x,
                                     std::size_t glob_y,
                                     std::size_t local_x,
                                     std::size_t local_y,
                                     std::uint8_t device_id)
    : Mouse_event{Event::MouseWheel, reciever, button,   glob_x, glob_y,
                  local_x,           local_y,  device_id} {}

void Mouse_wheel_event::send() const {
    reciever_->mouse_wheel_event(button_, glob_x_, glob_y_, local_x_, local_y_,
                                 device_id_);
}

Mouse_move_event::Mouse_move_event(Widget* reciever,
                                   Mouse_button button,
                                   std::size_t glob_x,
                                   std::size_t glob_y,
                                   std::size_t local_x,
                                   std::size_t local_y,
                                   std::uint8_t device_id)
    : Mouse_event{Event::MouseMove, reciever, button,  glob_x,
                  glob_y,           local_x,  local_y, device_id} {}

void Mouse_move_event::send() const {
    reciever_->mouse_move_event(button_, glob_x_, glob_y_, local_x_, local_y_,
                                device_id_);
}

}  // namespace cppurses;
