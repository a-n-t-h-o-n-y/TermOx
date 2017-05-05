#include "widget_module/widgets/checkbox.hpp"

#include "widget_module/size_policy.hpp"
#include "painter_module/color.hpp"
#include "painter_module/painter.hpp"

namespace twf {

Checkbox::Checkbox() {
    // this->enable_border();
    // this->brush().set_background(Color::Red);
    this->geometry().size_policy().horizontal_policy = Size_policy::Fixed;
    this->geometry().size_policy().vertical_policy = Size_policy::Fixed;
    this->geometry().set_width_hint(2);
    this->geometry().set_height_hint(1);
}

bool Checkbox::paint_event(const Paint_event& event) {
    Painter p{this};
    if (checked_) {
        p.put_at(0, 0, check_);
    } else {
        p.put_at(0, 0, empty_);
    }
    return Widget::paint_event(event);
}

bool Checkbox::mouse_press_event(const Mouse_event& event) {
    if (event.button() == Mouse_event::Button::LeftButton) {
        this->toggle();
        this->update();
    }
    return true;
}

void Checkbox::toggle() {
    checked_ = !checked_;
    toggled();
    if (checked_) {
        checked();
    } else {
        unchecked();
    }
}

}  // namespace twf
