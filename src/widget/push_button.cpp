#include "widget/widgets/push_button.hpp"
#include <utility>
#include "painter/glyph_string.hpp"
#include "painter/painter.hpp"
#include "system/mouse_button.hpp"

namespace cppurses {

Push_button::Push_button(Glyph_string name) : title_{std::move(name)} {}

void Push_button::set_text(Glyph_string text) {
    title_ = std::move(text);
    this->update();
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

namespace slot {

sig::Slot<void()> click(Push_button& pb) {
    sig::Slot<void()> slot{[&pb] {
        pb.clicked();
        pb.clicked_w_ref(&pb);
    }};
    slot.track(pb.destroyed);
    return slot;
}

}  // namespace slot
}  // namespace cppurses
