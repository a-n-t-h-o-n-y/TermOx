#ifndef SYSTEM_EVENT_HANDLER_HPP
#define SYSTEM_EVENT_HANDLER_HPP
#include <signals/signals.hpp>
#include <vector>

namespace cppurses {

class Event_handler {
   public:
    Event_handler(const Event_handler&) = delete;
    Event_handler& operator=(const Event_handler&) = delete;
    Event_handler(Event_handler&&) = delete;
    Event_handler& operator=(Event_handler&&) = delete;
    virtual ~Event_handler();

    void install_event_filter(Event_handler* filter);
    void remove_event_filter(Event_handler* filter);
    const std::vector<Event_handler*>& get_event_filters() const;

    // - - - - - - - - - - - - - Event Handlers - - - - - - - - - - - - - - - -
    virtual bool child_added_event(Event_handler* child) = 0;
    virtual bool child_removed_event(Event_handler* child) = 0;
    virtual bool child_polished_event(Event_handler* child) = 0;
    virtual bool enable_event();
    virtual bool disable_event();
    virtual bool move_event(std::size_t new_x,
                            std::size_t new_y,
                            std::size_t old_x,
                            std::size_t old_y) = 0;
    virtual bool resize_event(std::size_t new_width,
                              std::size_t new_height,
                              std::size_t old_width,
                              std::size_t old_height) = 0;
    virtual bool mouse_press_event(Mouse_button button,
                                   std::size_t global_x,
                                   std::size_t global_y,
                                   std::size_t local_x,
                                   std::size_t local_y,
                                   std::uint8_t device_id);
    virtual bool mouse_release_event(Mouse_button button,
                                     std::size_t global_x,
                                     std::size_t global_y,
                                     std::size_t local_x,
                                     std::size_t local_y,
                                     std::uint8_t device_id);
    virtual bool mouse_double_click_event(Mouse_button button,
                                          std::size_t global_x,
                                          std::size_t global_y,
                                          std::size_t local_x,
                                          std::size_t local_y,
                                          std::uint8_t device_id);
    virtual bool mouse_wheel_event(Mouse_button button,
                                   std::size_t global_x,
                                   std::size_t global_y,
                                   std::size_t local_x,
                                   std::size_t local_y,
                                   std::uint8_t device_id);
    virtual bool mouse_move_event(Mouse_button button,
                                  std::size_t global_x,
                                  std::size_t global_y,
                                  std::size_t local_x,
                                  std::size_t local_y,
                                  std::uint8_t device_id);
    virtual bool key_press_event(Key key, char symbol) = 0;
    virtual bool key_release_event(Key key, char symbol);
    virtual bool close_event() = 0;
    virtual bool hide_event();
    virtual bool show_event();
    virtual bool focus_in_event();
    virtual bool focus_out_event();
    virtual bool paint_event() = 0;
    virtual bool clear_screen_event() = 0;

    // - - - - - - - - - - - Event Filter Handlers - - - - - - - - - - - - - - -
    virtual bool child_added_event_filter(Event_handler* receiver,
                                          Event_handler* child);
    virtual bool child_removed_event_filter(Event_handler* receiver,
                                            Event_handler* child);
    virtual bool child_polished_event_filter(Event_handler* receiver,
                                             Event_handler* child);
    virtual bool enable_event_filter(Event_handler* receiver);
    virtual bool disable_event_filter(Event_handler* receiver);
    virtual bool move_event_filter(Event_handler* receiver,
                                   std::size_t new_x,
                                   std::size_t new_y,
                                   std::size_t old_x,
                                   std::size_t old_y);
    virtual bool resize_event_filter(Event_handler* receiver,
                                     std::size_t new_width,
                                     std::size_t new_height,
                                     std::size_t old_width,
                                     std::size_t old_height);
    virtual bool mouse_press_event_filter(Event_handler* receiver,
                                          Mouse_button button,
                                          std::size_t global_x,
                                          std::size_t global_y,
                                          std::size_t local_x,
                                          std::size_t local_y,
                                          std::uint8_t device_id);
    virtual bool mouse_release_event_filter(Event_handler* receiver,
                                            Mouse_button button,
                                            std::size_t global_x,
                                            std::size_t global_y,
                                            std::size_t local_x,
                                            std::size_t local_y,
                                            std::uint8_t device_id);
    virtual bool mouse_double_click_event_filter(Event_handler* receiver,
                                                 Mouse_button button,
                                                 std::size_t global_x,
                                                 std::size_t global_y,
                                                 std::size_t local_x,
                                                 std::size_t local_y,
                                                 std::uint8_t device_id);
    virtual bool mouse_wheel_event_filter(Event_handler* receiver,
                                          Mouse_button button,
                                          std::size_t global_x,
                                          std::size_t global_y,
                                          std::size_t local_x,
                                          std::size_t local_y,
                                          std::uint8_t device_id);
    virtual bool mouse_move_event_filter(Event_handler* receiver,
                                         Mouse_button button,
                                         std::size_t global_x,
                                         std::size_t global_y,
                                         std::size_t local_x,
                                         std::size_t local_y,
                                         std::uint8_t device_id);
    virtual bool key_press_event_filter(Event_handler* receiver,
                                        Key key,
                                        char symbol);
    virtual bool key_release_event_filter(Event_handler* receiver,
                                          Key key,
                                          char symbol);
    virtual bool close_event_filter(Event_handler* receiver);
    virtual bool hide_event_filter(Event_handler* receiver);
    virtual bool show_event_filter(Event_handler* receiver);
    virtual bool focus_in_event_filter(Event_handler* receiver);
    virtual bool focus_out_event_filter(Event_handler* receiver);
    virtual bool paint_event_filter(Event_handler* receiver);
    virtual bool clear_screen_event_filter(Event_handler* receiver);

    // Signals
    sig::Signal<void(Event_handler*)> destroyed;

   private:
    std::vector<Event_handler*> event_filters_;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENT_HANDLER_HPP
