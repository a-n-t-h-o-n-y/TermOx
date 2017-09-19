#include "widget/widgets/push_button.hpp"
#include "system/events/mouse_event.hpp"
#include "painter/glyph_string.hpp"
#include "painter/painter.hpp"
#include <utility>

namespace cppurses {

Push_button::Push_button(Glyph_string name) : title_{std::move(name)} {
    this->disable_cursor();
    click = [this] {
        clicked();
        clicked_w_ref(this);
    };
    click.track(this->destroyed);
}

bool Push_button::mouse_press_event(Mouse_button button,
                                    std::size_t global_x,
                                    std::size_t global_y,
                                    std::size_t local_x,
                                    std::size_t local_y,
                                    std::uint8_t device_id) {
    if (button == Mouse_button::Left) {
        clicked();
        clicked_w_ref(this);
    }
    return Widget::mouse_press_event(button, global_x, global_y, local_x,
                                     local_y, device_id);
}

bool Push_button::paint_event() {
    Painter p{this};
    auto widg_mid_point = this->width() / 2;
    auto title_mid_point = title_.size() / 2;
    auto x = widg_mid_point - title_mid_point;
    if (title_mid_point > widg_mid_point) {
        x = 0;
    }
    p.put(title_, x, this->height() / 2);
    return Widget::paint_event();
}

}  // namespace cppurses
