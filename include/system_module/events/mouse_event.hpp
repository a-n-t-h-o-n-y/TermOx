#ifndef SYSTEM_MODULE_EVENTS_MOUSE_EVENT_HPP
#define SYSTEM_MODULE_EVENTS_MOUSE_EVENT_HPP

#include "system_module/events/input_event.hpp"
#include <cstddef>
#include <cstdint>

namespace cppurses {

enum class Mouse_button { None, Left, Middle, Right, ScrollUp, ScrollDown };

class Mouse_event : public Input_event {
   public:
    Mouse_event(Event::Type type,
                Mouse_button button,
                std::size_t glob_x,
                std::size_t glob_y,
                std::size_t local_x,
                std::size_t local_y,
                std::uint8_t device_id)
        : Input_event{type},
          button_{button},
          glob_x_{glob_x},
          glob_y_{glob_y},
          local_x_{local_x},
          local_y_{local_y},
          device_id_{device_id} {}

    Mouse_button button() const { return button_; }
    std::size_t global_x() const { return glob_x_; }
    std::size_t global_y() const { return glob_y_; }
    std::size_t local_x() const { return local_x_; }
    std::size_t local_y() const { return local_y_; }
    std::int8_t device_id() const { return device_id_; }

   private:
    Mouse_button button_;
    std::size_t glob_x_;
    std::size_t glob_y_;
    std::size_t local_x_;
    std::size_t local_y_;
    std::uint8_t device_id_;
};

}  // namespace cppurses
#endif  // SYSTEM_MODULE_EVENTS_MOUSE_EVENT_HPP
