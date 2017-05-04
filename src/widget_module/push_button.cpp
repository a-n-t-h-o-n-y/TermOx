#include "widget_module/widgets/push_button.hpp"

#include "system_module/events/mouse_event.hpp"
#include <utility>

namespace twf {

Push_button::Push_button(Glyph_string name) : title_{std::move(name)} {
    this->set_cursor(false);
}

bool Push_button::mouse_press_event(const Mouse_event& event) {
    if (event.button() == Mouse_event::Button::LeftButton) {
        clicked();
    }
    return Widget::mouse_press_event(event);
}

bool Push_button::paint_event(const Paint_event& event) {
    Painter p{this};
    // Paint the visible sub-string.
    p.put_at(this->width() / 2 - (title_.size() / 2), this->height() / 2,
             title_, false);
    // Move the cursor to the appropriate position.
    return Widget::paint_event(event);
}

}  // namespace twf
