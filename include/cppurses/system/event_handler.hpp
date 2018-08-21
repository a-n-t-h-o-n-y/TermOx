#ifndef CPPURSES_SYSTEM_EVENT_HANDLER_HPP
#define CPPURSES_SYSTEM_EVENT_HANDLER_HPP
#include <cstddef>
#include <cstdint>
#include <vector>

#include <signals/signal.hpp>

#include <cppurses/system/key.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/point.hpp>

namespace cppurses {
class Widget;

class Event_handler {
   public:
    Event_handler() = default;
    Event_handler(const Event_handler&) = delete;
    Event_handler& operator=(const Event_handler&) = delete;
    Event_handler(Event_handler&&) = delete;
    Event_handler& operator=(Event_handler&&) = delete;
    virtual ~Event_handler();

    bool enabled() const;

    void install_event_filter(Event_handler* filter);
    void remove_event_filter(Event_handler* filter);
    const std::vector<Event_handler*>& get_event_filters() const;

    // - - - - - - - - - - - - - Event Handlers - - - - - - - - - - - - - - - -
    virtual bool enable_event();
    virtual bool disable_event();
    virtual bool child_added_event(Widget* child) = 0;
    virtual bool child_removed_event(Widget* child) = 0;
    virtual bool child_polished_event(Widget* child) = 0;
    virtual bool move_event(Point new_position, Point old_position) = 0;
    virtual bool resize_event(Area new_size, Area old_size) = 0;
    virtual bool mouse_press_event(Mouse_button button,
                                   Point global,
                                   Point local,
                                   std::uint8_t device_id);
    virtual bool mouse_release_event(Mouse_button button,
                                     Point global,
                                     Point local,
                                     std::uint8_t device_id);
    virtual bool mouse_double_click_event(Mouse_button button,
                                          Point global,
                                          Point local,
                                          std::uint8_t device_id);
    virtual bool mouse_wheel_event(Mouse_button button,
                                   Point global,
                                   Point local,
                                   std::uint8_t device_id);
    virtual bool mouse_move_event(Mouse_button button,
                                  Point global,
                                  Point local,
                                  std::uint8_t device_id);
    virtual bool key_press_event(Key key, char symbol);
    virtual bool key_release_event(Key key, char symbol);
    virtual bool focus_in_event();
    virtual bool focus_out_event();
    virtual bool delete_event();
    virtual bool paint_event() = 0;
    virtual bool animation_event() = 0;

    // - - - - - - - - - - - Event Filter Handlers - - - - - - - - - - - - - - -
    virtual bool child_added_event_filter(Event_handler* receiver,
                                          Widget* child);
    virtual bool child_removed_event_filter(Event_handler* receiver,
                                            Widget* child);
    virtual bool child_polished_event_filter(Event_handler* receiver,
                                             Widget* child);
    virtual bool enable_event_filter(Event_handler* receiver);
    virtual bool disable_event_filter(Event_handler* receiver);
    virtual bool move_event_filter(Event_handler* receiver,
                                   Point new_position,
                                   Point old_position);
    virtual bool resize_event_filter(Event_handler* receiver,
                                     Area new_size,
                                     Area old_size);
    virtual bool mouse_press_event_filter(Event_handler* receiver,
                                          Mouse_button button,
                                          Point global,
                                          Point local,
                                          std::uint8_t device_id);
    virtual bool mouse_release_event_filter(Event_handler* receiver,
                                            Mouse_button button,
                                            Point global,
                                            Point local,
                                            std::uint8_t device_id);
    virtual bool mouse_double_click_event_filter(Event_handler* receiver,
                                                 Mouse_button button,
                                                 Point global,
                                                 Point local,
                                                 std::uint8_t device_id);
    virtual bool mouse_wheel_event_filter(Event_handler* receiver,
                                          Mouse_button button,
                                          Point global,
                                          Point local,
                                          std::uint8_t device_id);
    virtual bool mouse_move_event_filter(Event_handler* receiver,
                                         Mouse_button button,
                                         Point global,
                                         Point local,
                                         std::uint8_t device_id);
    virtual bool key_press_event_filter(Event_handler* receiver,
                                        Key key,
                                        char symbol);
    virtual bool key_release_event_filter(Event_handler* receiver,
                                          Key key,
                                          char symbol);
    virtual bool focus_in_event_filter(Event_handler* receiver);
    virtual bool focus_out_event_filter(Event_handler* receiver);
    virtual bool delete_event_filter(Event_handler* receiver);
    virtual bool paint_event_filter(Event_handler* receiver);
    virtual bool animation_event_filter(Event_handler* receiver);

    // Signals
    sig::Signal<void(Event_handler*)> destroyed;
    sig::Signal<void(Point)> clicked;
    sig::Signal<void(std::size_t, std::size_t)> clicked_xy;
    sig::Signal<void(Point)> click_released;
    sig::Signal<void(std::size_t, std::size_t)> click_released_xy;
    sig::Signal<void(Point)> double_clicked;
    sig::Signal<void(std::size_t, std::size_t)> double_clicked_xy;
    sig::Signal<void(Key)> key_pressed;
    sig::Signal<void(Key)> key_released;

   protected:
    bool enabled_{false};

   private:
    std::vector<Event_handler*> event_filters_;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENT_HANDLER_HPP
