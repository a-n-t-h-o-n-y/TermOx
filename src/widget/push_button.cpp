#include <cppurses/widget/widgets/push_button.hpp>

#include <cstddef>
#include <utility>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/system/mouse_data.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

Push_button::Push_button(Glyph_string label) : label_{std::move(label)} {
    this->set_name("Push_button");
}

void Push_button::set_label(Glyph_string label) {
    label_ = std::move(label);
    this->update();
}

bool Push_button::mouse_press_event(const Mouse_data& mouse) {
    if (mouse.button == Mouse_button::Left) {
        clicked();
    }
    return Widget::mouse_press_event(mouse);
}

bool Push_button::paint_event() {
    const auto width = this->width();
    const auto x = label_.size() > width ? 0 : (width - label_.size()) / 2;
    const auto y = this->height() / 2;
    Painter p{*this};
    p.put(label_, x, y);
    return Widget::paint_event();
}
}  // namespace cppurses
