#include "widget_module/layout.hpp"
#include "painter_module/painter.hpp"
#include "system_module/events/child_event.hpp"
#include "system_module/events/paint_event.hpp"

namespace twf {

Layout::Layout() {
    this->set_x(0);
    this->set_y(0);
    this->geometry().set_width(System::max_width());
    this->geometry().set_height(System::max_height());
    Layout::initialize();
}

void Layout::initialize() {
    this->update_layout = [this]{this->update();};
    this->update_layout.track(this->destroyed);
}

bool Layout::paint_event(const Paint_event& event) {
    too_small_ = false;
    this->update_geometry();
    if (too_small_) {
        Painter p{this};
        p.put_at(0, 0, "Screen too small.");
    }
    return Widget::paint_event(event);
}

bool Layout::child_event(const Child_event& event) {
    this->update();    
    return Widget::child_event(event);
}

}  // namespace twf
