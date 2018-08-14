#include <cppurses/widget/layout.hpp>

#include <cppurses/painter/painter.hpp>
#include <cppurses/system/events/move_event.hpp>
#include <cppurses/system/events/resize_event.hpp>
#include <cppurses/system/system.hpp>

namespace cppurses {

Layout::Layout() {
    // TODO is this necessary? To post a move event at construction?
    System::post_event<Move_event>(this, Point{0, 0});
    this->focus_policy = Focus_policy::Click;
}

bool Layout::child_added_event(Widget* child) {
    needs_update_ = true;
    return Widget::child_added_event(child);
}

bool Layout::child_removed_event(Widget* child) {
    needs_update_ = true;
    return Widget::child_removed_event(child);
}

bool Layout::child_polished_event(Widget* child) {
    needs_update_ = true;
    return Widget::child_polished_event(child);
}

bool Layout::paint_event() {
    too_small_ = false;
    if (needs_update_ || last_inner_position_.x != this->inner_x() ||
        last_inner_position_.y != this->inner_y() ||
        last_inner_size_.width != this->width() ||
        last_inner_size_.height != this->height()) {
        this->update_geometry();
    }
    if (too_small_) {
        Painter p{this};
        // need to set repaint on this, but that will not do anything because
        // flush will optimize it out because of children.
        p.put("Screen too small.", 0, 0);
    }
    last_inner_position_ = Point{this->x(), this->y()};
    last_inner_size_ = Area{this->width(), this->height()};
    needs_update_ = false;
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
