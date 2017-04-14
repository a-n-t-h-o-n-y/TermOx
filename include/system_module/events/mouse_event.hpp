#ifndef MOUSE_EVENT_HPP
#define MOUSE_EVENT_HPP

#include "input_event.hpp"

#include <cstddef>
#include <cstdint>

namespace twf {

class Mouse_event : public Input_event {
   public:
    enum class Button {// maybe move outside of Mouse_event class.
        NoButton,
        LeftButton,  // BUTTON1
        MidButton,
        RightButton,
        ScrollUp,
        ScrollDown
        // ... add ?
    };

    Mouse_event(Event::Type type,
                Button button,
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

    Button button() const { return button_; }
    std::size_t global_x() const { return glob_x_; }
    std::size_t global_y() const { return glob_y_; }
    std::size_t local_x() const { return local_x_; }
    std::size_t local_y() const { return local_y_; }
    std::int8_t device_id() const { return device_id_; }

   private:
    Button button_;
    std::size_t glob_x_;
    std::size_t glob_y_;
    std::size_t local_x_;
    std::size_t local_y_;
    std::uint8_t device_id_;
};

}  // namespace twf
#endif  // MOUSE_EVENT_HPP
