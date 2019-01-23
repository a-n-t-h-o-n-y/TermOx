#include "offset_control_widget.hpp"

#include <utility>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/system/mouse_data.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widget.hpp>

namespace gol {

Vertical_arrows::Vertical_arrows() {
    up_btn.install_event_filter(*this);
    down_btn.install_event_filter(*this);
}

bool Vertical_arrows::mouse_press_event_filter(
    cppurses::Widget& receiver,
    const cppurses::Mouse_data& mouse) {
    if (mouse.button == cppurses::Mouse_button::ScrollUp) {
        up_btn.clicked();
        return true;
    } else if (mouse.button == cppurses::Mouse_button::ScrollDown) {
        down_btn.clicked();
        return true;
    }
    return false;
}

Scroll_btn::Scroll_btn(cppurses::Glyph_string title)
    : Push_button{std::move(title)} {}

bool Scroll_btn::mouse_press_event(const cppurses::Mouse_data& mouse) {
    if (mouse.button == cppurses::Mouse_button::ScrollUp) {
        scrolled_up();
    } else if (mouse.button == cppurses::Mouse_button::ScrollDown) {
        scrolled_down();
    }
    return Push_button::mouse_press_event(mouse);
}

Horizontal_arrow::Horizontal_arrow(cppurses::Glyph_string title)
    : arrow{this->make_child<Scroll_btn>(title)} {
    this->width_policy.type(cppurses::Size_policy::Fixed);
    this->width_policy.hint(5);
    space.brush.set_background(cppurses::Color::Gray);
}

Offset_control_widget::Offset_control_widget() {
    this->height_policy.type(cppurses::Size_policy::Fixed);
    this->height_policy.hint(2);
    left_arrow.arrow.scrolled_up.connect([this]() { right_request(); });
    left_arrow.arrow.scrolled_down.connect([this]() { left_request(); });
    right_arrow.arrow.scrolled_up.connect([this]() { right_request(); });
    right_arrow.arrow.scrolled_down.connect([this]() { left_request(); });
}
}  // namespace gol
