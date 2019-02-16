#include <cppurses/widget/widget.hpp>

#include <cstdint>

#include <cppurses/painter/painter.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/keyboard_data.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/point.hpp>

namespace cppurses {

// - - - - - - - - - - - - - - Event Handlers - - - - - - - - - - - - - - - - -
bool Widget::enable_event() {
    return true;
}

bool Widget::disable_event() {
    if (this == Focus::focus_widget()) {
        Focus::clear_focus();
    }
    return true;
}

bool Widget::child_added_event(Widget& child) {
    child_added(&child);
    this->update();
    return true;
}

bool Widget::child_removed_event(Widget& child) {
    child_removed(&child);
    this->update();
    return true;
}

bool Widget::child_polished_event(Widget& child) {
    this->update();
    return true;
}

bool Widget::move_event(Point new_position, Point old_position) {
    moved(new_position);
    this->update();
    return true;
}

bool Widget::resize_event(Area new_size, Area old_size) {
    resized(outer_width_, outer_height_);
    this->update();
    return true;
}

bool Widget::mouse_press_event(const Mouse::State& mouse) {
    clicked(mouse.local);
    clicked_xy(mouse.local.x, mouse.local.y);
    return true;
}

bool Widget::mouse_release_event(const Mouse::State& mouse) {
    click_released(mouse.global);
    click_released_xy(mouse.global.x, mouse.global.y);
    return true;
}

bool Widget::mouse_double_click_event(const Mouse::State& mouse) {
    double_clicked(mouse.global);
    double_clicked_xy(mouse.global.x, mouse.global.y);
    return true;
}

bool Widget::mouse_wheel_event(const Mouse::State& mouse) {
    return false;
}

bool Widget::mouse_move_event(const Mouse::State& mouse) {
    return false;
}

bool Widget::key_press_event(const Keyboard_data& keyboard) {
    key_pressed(keyboard.key);
    return true;
}

bool Widget::key_release_event(const Keyboard_data& keyboard) {
    key_released(keyboard.key);
    return false;
}

bool Widget::focus_in_event() {
    focused_in();
    return true;
}

bool Widget::focus_out_event() {
    focused_out();
    return true;
}

bool Widget::delete_event() {
    return true;
}

bool Widget::paint_event() {
    Painter p{*this};
    p.border();
    return true;
}

bool Widget::timer_event() {
    this->update();
    return true;
}

// - - - - - - - - - - - - Event Filter Handlers - - - - - - - - - - - - - - - -

bool Widget::child_added_event_filter(Widget& receiver, Widget& child) {
    return false;
}

bool Widget::child_removed_event_filter(Widget& receiver, Widget& child) {
    return false;
}

bool Widget::child_polished_event_filter(Widget& receiver, Widget& child) {
    return false;
}

bool Widget::enable_event_filter(Widget& receiver) {
    return false;
}

bool Widget::disable_event_filter(Widget& receiver) {
    return false;
}

bool Widget::move_event_filter(Widget& receiver,
                               Point new_position,
                               Point old_position) {
    return false;
}

bool Widget::resize_event_filter(Widget& receiver,
                                 Area new_size,
                                 Area old_size) {
    return false;
}

bool Widget::mouse_press_event_filter(Widget& receiver,
                                      const Mouse::State& mouse) {
    return false;
}

bool Widget::mouse_release_event_filter(Widget& receiver,
                                        const Mouse::State& mouse) {
    return false;
}

bool Widget::mouse_double_click_event_filter(Widget& receiver,
                                             const Mouse::State& mouse) {
    return false;
}

bool Widget::mouse_wheel_event_filter(Widget& receiver,
                                      const Mouse::State& mouse) {
    return false;
}

bool Widget::mouse_move_event_filter(Widget& receiver,
                                     const Mouse::State& mouse) {
    return false;
}

bool Widget::key_press_event_filter(Widget& receiver,
                                    const Keyboard_data& keyboard) {
    return false;
}

bool Widget::key_release_event_filter(Widget& receiver,
                                      const Keyboard_data& keyboard) {
    return false;
}

bool Widget::focus_in_event_filter(Widget& receiver) {
    return false;
}

bool Widget::focus_out_event_filter(Widget& receiver) {
    return false;
}

bool Widget::delete_event_filter(Widget& receiver) {
    return false;
}

bool Widget::paint_event_filter(Widget& receiver) {
    return false;
}

bool Widget::timer_event_filter(Widget& receiver) {
    return false;
}

}  // namespace cppurses
