#include "system/event_handler.hpp"
#include <signals/signals.hpp>
#include <cstddef>
#include <vector>

namespace cppurses {

Event_handler::~Event_handler() {
    if (valid_) {
        destroyed(this);
    }
}

void Event_handler::install_event_filter(Event_handler* filter) {
    if (filter == this) {
        return;
    }
    // Remove filter from list on destruction of filter
    Signals::Slot<void()> remove_on_destroy{
        [this, filter] { this->remove_event_filter(filter); }};
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

const std::vector<Event_handler*>& Event_filter::get_event_filters() const {
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
    return false;
}

bool Event_handler::mouse_release_event(Mouse_button button,
                                        std::size_t global_x,
                                        std::size_t global_y,
                                        std::size_t local_x,
                                        std::size_t local_y,
                                        std::uint8_t device_id) {
    return false;
}

bool Event_handler::mouse_double_click_event(Mouse_button button,
                                             std::size_t global_x,
                                             std::size_t global_y,
                                             std::size_t local_x,
                                             std::size_t local_y,
                                             std::uint8_t device_id) {
    return false;
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

bool Event_handler::key_release_event(Key key, char symbol) {
    return false;
}

bool Event_handler::hide_event() {
    return true;
}

bool Event_handler::show_event() {
    return true;
}

bool Event_handler::focus_in_event() {
    return true;
}

bool Event_handler::focus_out_event() {
    return true;
}

// - - - - - - - - - - - - Event Filter Handlers - - - - - - - - - - - - - - - -
bool Event_handler::child_added_event_filter(Event_handler* receiver,
                                             Event_handler* child) {
    return false;
}

bool Event_handler::child_removed_event_filter(Event_handler* receiver,
                                               Event_handler* child) {
    return false;
}

bool Event_handler::child_polished_event_filter(Event_handler* receiver,
                                                Event_handler* child) {
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

bool Event_handler::focus_in_event_filter(Event_handler* receiver) {
    return false;
}

bool Event_handler::focus_out_event_filter(Event_handler* receiver) {
    return false;
}

bool Event_handler::paint_event_filter(Event_handler* receiver) {
    return false;
}

bool Event_handler::clear_screen_event_filter(Event_handler* receiver) {
    return false;
}

}  // namespace cppurses
