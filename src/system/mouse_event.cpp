#include <cppurses/system/events/mouse_event.hpp>

#include <cstdint>

#include <cppurses/system/event.hpp>
#include <cppurses/system/event_handler.hpp>
#include <cppurses/system/events/input_event.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

// class Mouse_event
Mouse_event::Mouse_event(Event::Type type,
                         Event_handler* receiver,
                         Mouse_button button,
                         Point global,
                         Point local,
                         std::uint8_t device_id)
    : Input_event{type, receiver},
      button_{button},
      global_{global},
      local_{local},
      device_id_{device_id} {}

// class Mouse_press_event
Mouse_press_event::Mouse_press_event(Event_handler* receiver,
                                     Mouse_button button,
                                     Point global,
                                     Point local,
                                     std::uint8_t device_id)
    : Mouse_event{Event::MouseButtonPress,
                  receiver,
                  button,
                  global,
                  local,
                  device_id} {}

bool Mouse_press_event::send() const {
    // Handle focus elsewhere.
    if (receiver_->enabled()) {
        Focus::mouse_press(static_cast<Widget*>(receiver_));
        return receiver_->mouse_press_event(button_, global_, local_,
                                            device_id_);
    }
    return false;
}

bool Mouse_press_event::filter_send(Event_handler* filter) const {
    return filter->mouse_press_event_filter(receiver_, button_, global_, local_,
                                            device_id_);
}

// class Mouse_release_event
Mouse_release_event::Mouse_release_event(Event_handler* receiver,
                                         Mouse_button button,
                                         Point global,
                                         Point local,
                                         std::uint8_t device_id)
    : Mouse_event{Event::MouseButtonRelease,
                  receiver,
                  button,
                  global,
                  local,
                  device_id} {}

bool Mouse_release_event::send() const {
    if (receiver_->enabled()) {
        return receiver_->mouse_release_event(button_, global_, local_,
                                              device_id_);
    }
    return false;
}

bool Mouse_release_event::filter_send(Event_handler* filter) const {
    return filter->mouse_release_event_filter(receiver_, button_, global_,
                                              local_, device_id_);
}

// class Mouse_double_click_event
Mouse_double_click_event::Mouse_double_click_event(Event_handler* receiver,
                                                   Mouse_button button,
                                                   Point global,
                                                   Point local,
                                                   std::uint8_t device_id)
    : Mouse_event{Event::MouseButtonDblClick,
                  receiver,
                  button,
                  global,
                  local,
                  device_id} {}

bool Mouse_double_click_event::send() const {
    if (receiver_->enabled()) {
        return receiver_->mouse_double_click_event(button_, global_, local_,
                                                   device_id_);
    }
    return false;
}

bool Mouse_double_click_event::filter_send(Event_handler* filter) const {
    return filter->mouse_double_click_event_filter(receiver_, button_, global_,
                                                   local_, device_id_);
}

// class Mouse_wheel_event
Mouse_wheel_event::Mouse_wheel_event(Event_handler* receiver,
                                     Mouse_button button,
                                     Point global,
                                     Point local,
                                     std::uint8_t device_id)
    : Mouse_event{Event::MouseWheel, receiver, button, global, local,
                  device_id} {}

bool Mouse_wheel_event::send() const {
    if (receiver_->enabled()) {
        return receiver_->mouse_wheel_event(button_, global_, local_,
                                            device_id_);
    }
    return false;
}

bool Mouse_wheel_event::filter_send(Event_handler* filter) const {
    return filter->mouse_wheel_event_filter(receiver_, button_, global_, local_,
                                            device_id_);
}

// class Mouse_move_event
Mouse_move_event::Mouse_move_event(Event_handler* receiver,
                                   Mouse_button button,
                                   Point global,
                                   Point local,
                                   std::uint8_t device_id)
    : Mouse_event{Event::MouseMove, receiver, button,
                  global,           local,    device_id} {}

bool Mouse_move_event::send() const {
    if (receiver_->enabled()
        // && static_cast<Widget*>(receiver_)->has_mouse_tracking()
    ) {
        return receiver_->mouse_move_event(button_, global_, local_,
                                           device_id_);
    }
    return false;
}

bool Mouse_move_event::filter_send(Event_handler* filter) const {
    return filter->mouse_move_event_filter(receiver_, button_, global_, local_,
                                           device_id_);
}

}  // namespace cppurses
