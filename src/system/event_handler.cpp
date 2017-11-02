#include <cppurses/system/event_handler.hpp>
#include <cppurses/system/events/disable_event.hpp>
#include <cppurses/system/events/enable_event.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/point.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <signals/signals.hpp>
#include <vector>

namespace cppurses {
class Widget;

Event_handler::~Event_handler() {
    destroyed(this);
}

void Event_handler::set_enabled(bool enabled) {
    enabled_ = enabled;
    if (enabled) {
        System::post_event<Enable_event>(this);
    } else {
        System::post_event<Disable_event>(this);
    }
}

bool Event_handler::enabled() const {
    return enabled_;
}

void Event_handler::install_event_filter(Event_handler* filter) {
    if (filter == this) {
        return;
    }
    // Remove filter from list on destruction of filter
    sig::Slot<void(Event_handler*)> remove_on_destroy{
        [this](Event_handler* being_destroyed) {
            this->remove_event_filter(being_destroyed);
        }};
    remove_on_destroy.track(this->destroyed);
    filter->destroyed.connect(remove_on_destroy);
    event_filters_.push_back(filter);
}

void Event_handler::remove_event_filter(Event_handler* filter) {
    auto begin = std::begin(event_filters_);
    auto end = std::end(event_filters_);
    auto position = std::find(begin, end, filter);
    if (position != end) {
        event_filters_.erase(position);
    }
}

const std::vector<Event_handler*>& Event_handler::get_event_filters() const {
    return event_filters_;
}

// - - - - - - - - - - - - - - Event Handlers - - - - - - - - - - - - - - - - -
bool Event_handler::enable_event() {
    return false;
}

bool Event_handler::disable_event() {
    return false;
}

bool Event_handler::mouse_press_event(Mouse_button button,
                                      std::size_t global_x,
                                      std::size_t global_y,
                                      std::size_t local_x,
                                      std::size_t local_y,
                                      std::uint8_t device_id) {
    clicked(Point{global_x, global_y});
    clicked_xy(global_x, global_y);
    return true;
}

bool Event_handler::mouse_release_event(Mouse_button button,
                                        std::size_t global_x,
                                        std::size_t global_y,
                                        std::size_t local_x,
                                        std::size_t local_y,
                                        std::uint8_t device_id) {
    click_released(Point{global_x, global_y});
    click_released_xy(global_x, global_y);
    return true;
}

bool Event_handler::mouse_double_click_event(Mouse_button button,
                                             std::size_t global_x,
                                             std::size_t global_y,
                                             std::size_t local_x,
                                             std::size_t local_y,
                                             std::uint8_t device_id) {
    double_clicked(Point{global_x, global_y});
    double_clicked_xy(global_x, global_y);
    return true;
}

bool Event_handler::mouse_wheel_event(Mouse_button button,
                                      std::size_t global_x,
                                      std::size_t global_y,
                                      std::size_t local_x,
                                      std::size_t local_y,
                                      std::uint8_t device_id) {
    return false;
}

bool Event_handler::mouse_move_event(Mouse_button button,
                                     std::size_t global_x,
                                     std::size_t global_y,
                                     std::size_t local_x,
                                     std::size_t local_y,
                                     std::uint8_t device_id) {
    return false;
}

bool Event_handler::key_press_event(Key key, char symbol) {
    key_pressed(key);
    return true;
}

bool Event_handler::key_release_event(Key key, char symbol) {
    key_released(key);
    return false;
}

bool Event_handler::focus_in_event() {
    return true;
}

bool Event_handler::focus_out_event() {
    return true;
}

// - - - - - - - - - - - - Event Filter Handlers - - - - - - - - - - - - - - - -
bool Event_handler::child_added_event_filter(Event_handler* receiver,
                                             Widget* child) {
    return false;
}

bool Event_handler::child_removed_event_filter(Event_handler* receiver,
                                               Widget* child) {
    return false;
}

bool Event_handler::child_polished_event_filter(Event_handler* receiver,
                                                Widget* child) {
    return false;
}

bool Event_handler::enable_event_filter(Event_handler* receiver) {
    return false;
}

bool Event_handler::disable_event_filter(Event_handler* receiver) {
    return false;
}

bool Event_handler::move_event_filter(Event_handler* receiver,
                                      std::size_t new_x,
                                      std::size_t new_y,
                                      std::size_t old_x,
                                      std::size_t old_y) {
    return false;
}

bool Event_handler::resize_event_filter(Event_handler* receiver,
                                        std::size_t new_width,
                                        std::size_t new_height,
                                        std::size_t old_width,
                                        std::size_t old_height) {
    return false;
}

bool Event_handler::mouse_press_event_filter(Event_handler* receiver,
                                             Mouse_button button,
                                             std::size_t global_x,
                                             std::size_t global_y,
                                             std::size_t local_x,
                                             std::size_t local_y,
                                             std::uint8_t device_id) {
    return false;
}

bool Event_handler::mouse_release_event_filter(Event_handler* receiver,
                                               Mouse_button button,
                                               std::size_t global_x,
                                               std::size_t global_y,
                                               std::size_t local_x,
                                               std::size_t local_y,
                                               std::uint8_t device_id) {
    return false;
}

bool Event_handler::mouse_double_click_event_filter(Event_handler* receiver,
                                                    Mouse_button button,
                                                    std::size_t global_x,
                                                    std::size_t global_y,
                                                    std::size_t local_x,
                                                    std::size_t local_y,
                                                    std::uint8_t device_id) {
    return false;
}

bool Event_handler::mouse_wheel_event_filter(Event_handler* receiver,
                                             Mouse_button button,
                                             std::size_t global_x,
                                             std::size_t global_y,
                                             std::size_t local_x,
                                             std::size_t local_y,
                                             std::uint8_t device_id) {
    return false;
}

bool Event_handler::mouse_move_event_filter(Event_handler* receiver,
                                            Mouse_button button,
                                            std::size_t global_x,
                                            std::size_t global_y,
                                            std::size_t local_x,
                                            std::size_t local_y,
                                            std::uint8_t device_id) {
    return false;
}

bool Event_handler::key_press_event_filter(Event_handler* receiver,
                                           Key key,
                                           char symbol) {
    return false;
}

bool Event_handler::key_release_event_filter(Event_handler* receiver,
                                             Key key,
                                             char symbol) {
    return false;
}

bool Event_handler::close_event_filter(Event_handler* receiver) {
    return false;
}

bool Event_handler::hide_event_filter(Event_handler* receiver) {
    return false;
}

bool Event_handler::show_event_filter(Event_handler* receiver) {
    return false;
}

bool Event_handler::on_tree_event_filter(Event_handler* receiver,
                                         bool on_tree) {
    return false;
}

bool Event_handler::focus_in_event_filter(Event_handler* receiver) {
    return false;
}

bool Event_handler::focus_out_event_filter(Event_handler* receiver) {
    return false;
}

bool Event_handler::deferred_delete_event_filter(Event_handler* receiver,
                                                 Event_handler* to_delete) {
    return false;
}

bool Event_handler::paint_event_filter(Event_handler* receiver) {
    return false;
}

bool Event_handler::clear_screen_event_filter(Event_handler* receiver) {
    return false;
}

}  // namespace cppurses
