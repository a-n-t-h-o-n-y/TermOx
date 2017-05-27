#include "widget_module/layout.hpp"
#include "painter_module/painter.hpp"
#include "system_module/system.hpp"

namespace twf {
class Paint_event;

Layout::Layout() {
    this->set_x(0);
    this->set_y(0);
    this->geometry().set_width(System::max_width());
    this->geometry().set_height(System::max_height());
}

bool Layout::paint_event(const Paint_event& event) {
    too_small_ = false;
    this->update_geometry();
    if (too_small_) {
        Painter p{this};
        p.put("Screen too small.");
    }
    return Widget::paint_event(event);
}

}  // namespace twf
