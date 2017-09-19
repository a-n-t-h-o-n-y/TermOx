#include "system/events/mouse_event.hpp"
#include "widget/widget.hpp"
#include <cstddef>
#include <cstdint>

namespace cppurses {
class Widget;

// class Mouse_event
Mouse_event::Mouse_event(Event::Type type,
                         Widget* receiver,
                         Mouse_button button,
                         std::size_t glob_x,
                         std::size_t glob_y,
                         std::size_t local_x,
                         std::size_t local_y,
                         std::uint8_t device_id)
    : Input_event{type, receiver},
      button_{button},
      glob_x_{glob_x},
      glob_y_{glob_y},
      local_x_{local_x},
      local_y_{local_y},
      device_id_{device_id} {}

// class Mouse_press_event
Mouse_press_event::Mouse_press_event(Widget* receiver,
                                     Mouse_button button,
                                     std::size_t glob_x,
                                     std::size_t glob_y,
                                     std::size_t local_x,
                                     std::size_t local_y,
                                     std::uint8_t device_id)
    : Mouse_event{Event::MouseButtonPress,
                  receiver,
                  button,
                  glob_x,
                  glob_y,
                  local_x,
                  local_y,
                  device_id} {}

bool Mouse_press_event::send() const {
    // Handle focus elsewhere.
    if (receiver_->visible() && receiver_->enabled()) {
        if (receiver->focus_policy() == Focus_policy::Click ||
            receiver->focus_policy() == Focus_policy::Strong) {
            System::set_focus_widget(receiver);
            receiver_->paint_engine().move(
                receiver_->x() + receiver_->cursor_x(),
                receiver_->y() + receiver_->cursor_y());
        }
        return receiver_->mouse_press_event(button_, glob_x_, glob_y_, local_x_,
                                            local_y_, device_id_);
    }
    return false;
}

bool Mouse_press_event::filter_send(Widget* filter_widget) const {
    return filter_widget->mouse_press_event_filter(
        receiver_, button_, glob_x_, glob_y_, local_x_, local_y_, device_id);
}

// class Mouse_release_event
Mouse_release_event::Mouse_release_event(Widget* receiver,
                                         Mouse_button button,
                                         std::size_t glob_x,
                                         std::size_t glob_y,
                                         std::size_t local_x,
                                         std::size_t local_y,
                                         std::uint8_t device_id)
    : Mouse_event{Event::MouseButtonRelease,
                  receiver,
                  button,
                  glob_x,
                  glob_y,
                  local_x,
                  local_y,
                  device_id} {}

bool Mouse_release_event::send() const {
    if (receiver_->visible() && receiver_->enabled()) {
        return receiver_->mouse_release_event(button_, glob_x_, glob_y_,
                                              local_x_, local_y_, device_id_);
    }
    return false;
}

bool Mouse_release_event::filter_send(Widget* filter_widget) const {
    return filter_widget->mouse_release_event_filter(
        receiver_, button_, glob_x_, glob_y_, local_x_, local_y_, device_id);
}

// class Mouse_double_click_event
Mouse_double_click_event::Mouse_double_click_event(Widget* receiver,
                                                   Mouse_button button,
                                                   std::size_t glob_x,
                                                   std::size_t glob_y,
                                                   std::size_t local_x,
                                                   std::size_t local_y,
                                                   std::uint8_t device_id)
    : Mouse_event{Event::MouseButtonDblClick,
                  receiver,
                  button,
                  glob_x,
                  glob_y,
                  local_x,
                  local_y,
                  device_id} {}

bool Mouse_double_click_event::send() const {
    if (receiver_->visible() && receiver_->enabled()) {
        return receiver_->mouse_double_click_event(
            button_, glob_x_, glob_y_, local_x_, local_y_, device_id_);
    }
    return false;
}

bool Mouse_double_click_event::filter_send(Widget* filter_widget) const {
    return filter_widget->mouse_double_click_event_filter(
        receiver_, button_, glob_x_, glob_y_, local_x_, local_y_, device_id);
}

// class Mouse_wheel_event
Mouse_wheel_event::Mouse_wheel_event(Widget* receiver,
                                     Mouse_button button,
                                     std::size_t glob_x,
                                     std::size_t glob_y,
                                     std::size_t local_x,
                                     std::size_t local_y,
                                     std::uint8_t device_id)
    : Mouse_event{Event::MouseWheel, receiver, button,   glob_x, glob_y,
                  local_x,           local_y,  device_id} {}

bool Mouse_wheel_event::send() const {
    if (receiver_->visible() && receiver_->enabled()) {
        return receiver_->mouse_wheel_event(button_, glob_x_, glob_y_, local_x_,
                                            local_y_, device_id_);
    }
    return false;
}

bool Mouse_wheel_event::filter_send(Widget* filter_widget) const {
    return filter_widget->mouse_wheel_event_filter(
        receiver_, button_, glob_x_, glob_y_, local_x_, local_y_, device_id);
}

// class Mouse_move_event
Mouse_move_event::Mouse_move_event(Widget* receiver,
                                   Mouse_button button,
                                   std::size_t glob_x,
                                   std::size_t glob_y,
                                   std::size_t local_x,
                                   std::size_t local_y,
                                   std::uint8_t device_id)
    : Mouse_event{Event::MouseMove, receiver, button,  glob_x,
                  glob_y,           local_x,  local_y, device_id} {}

bool Mouse_move_event::send() const {
    if (receiver_->visible() && receiver_->enabled()) {
        return receiver_->mouse_move_event(button_, glob_x_, glob_y_, local_x_,
                                           local_y_, device_id_);
    }
    return false;
}

bool Mouse_move_event::filter_send(Widget* filter_widget) const {
    return filter_widget->mouse_move_event_filter(
        receiver_, button_, glob_x_, glob_y_, local_x_, local_y_, device_id);
}

}  // namespace cppurses;
