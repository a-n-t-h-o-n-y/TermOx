#include <cppurses/widget/layout.hpp>

#include <cppurses/painter/painter.hpp>
#include <cppurses/system/events/move_event.hpp>
#include <cppurses/system/events/resize_event.hpp>
#include <cppurses/system/system.hpp>

namespace cppurses {

bool Layout::move_event(Point new_position, Point old_position) {
    // shift children positions with move events, without sending resize events
    this->update_geometry();
    return Widget::move_event(new_position, old_position);
}

bool Layout::resize_event(Area new_size, Area old_size) {
    this->update_geometry();
    return Widget::resize_event(new_size, old_size);
}

bool Layout::child_added_event(Widget* child) {
    this->update_geometry();
    return Widget::child_added_event(child);
}

bool Layout::child_removed_event(Widget* child) {
    this->update_geometry();
    return Widget::child_removed_event(child);
}

bool Layout::child_polished_event(Widget* child) {
    this->update_geometry();
    return Widget::child_polished_event(child);
}

bool Layout::paint_event() {
    // TODO get rid of this and remove event
    this->screen_state().is_layout = true;
    return Widget::paint_event();
}

// Free Functions
void set_background(Layout& l, Color c) {
    for (const std::unique_ptr<Widget>& w : l.children.get()) {
        set_background(*w, c);
    }
}

void set_foreground(Layout& l, Color c) {
    for (const std::unique_ptr<Widget>& w : l.children.get()) {
        set_foreground(*w, c);
    }
}

}  // namespace cppurses
