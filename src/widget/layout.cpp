#include "widget/layout.hpp"
#include "painter/painter.hpp"
#include "system/system.hpp"
#include "system/events/move_event.hpp"
#include "system/events/resize_event.hpp"

namespace cppurses {

Layout::Layout() {
    System::post_event<Move_event>(this, 0, 0);
}

bool Layout::paint_event() {
    too_small_ = false;
    this->update_geometry();
    if (too_small_) {
        Painter p{this};
        p.put("Screen too small.");
    }
    return Widget::paint_event();
}

}  // namespace cppurses
