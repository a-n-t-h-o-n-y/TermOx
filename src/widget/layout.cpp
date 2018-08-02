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

bool Layout::paint_event() {
    too_small_ = false;
    this->update_geometry();
    if (too_small_) {
        Painter p{this};
        p.put("Screen too small.", 0, 0);
    }
    return Widget::paint_event();
}

// Free Functions
void set_background(Layout& l, Color c) {
    for (const std::unique_ptr<Widget>& w : l.children()) {
        set_background(*w, c);
    }
}

void set_foreground(Layout& l, Color c) {
    for (const std::unique_ptr<Widget>& w : l.children()) {
        set_foreground(*w, c);
    }
}

}  // namespace cppurses
