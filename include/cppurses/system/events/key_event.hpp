#ifndef SYSTEM_EVENTS_KEY_EVENT_HPP
#define SYSTEM_EVENTS_KEY_EVENT_HPP

#include "system/events/input_event.hpp"
#include "system/key.hpp"

namespace cppurses {
class Widget;

class Key_event : public Input_event {
   public:
    Key_event(Event::Type type, Widget* receiver, Key key_code);

   protected:
    const Key key_code_;
};

class Key_press_event : public Key_event {
   public:
    Key_press_event(Widget* receiver, Key key_code);
    bool send() const override;
    bool filter_send(Widget* filter_widget) const override;
};

class Key_release_event : public Key_event {
   public:
    Key_release_event(Widget* receiver, Key key_code);
    bool send() const override;
    bool filter_send(Widget* filter_widget) const override;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENTS_KEY_EVENT_HPP
