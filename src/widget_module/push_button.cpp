#include "widget_module/widgets/push_button.hpp"
#include "system_module/events/mouse_event.hpp"
#include "painter_module/glyph_string.hpp"
#include "painter_module/painter.hpp"
#include <utility>

namespace twf {

Push_button::Push_button(Glyph_string name) : title_{std::move(name)} {
    this->disable_cursor();
}

bool Push_button::mouse_press_event(const Mouse_event& event) {
    if (event.button() == Mouse_button::Left) {
        clicked();
    }
    return Widget::mouse_press_event(event);
}

bool Push_button::paint_event(const Paint_event& event) {
    Painter p{this};
    p.put(title_, this->width() / 2 - (title_.size() / 2),
             this->height() / 2);
    return Widget::paint_event(event);
}

}  // namespace twf
