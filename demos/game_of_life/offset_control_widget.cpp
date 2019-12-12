#include "offset_control_widget.hpp"

#include <utility>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/widget.hpp>

using namespace cppurses;
namespace gol {

Vertical_arrows::Vertical_arrows()
{
    up_btn.install_event_filter(*this);
    down_btn.install_event_filter(*this);
    up_btn.brush.set_background(Color::Light_gray);
    down_btn.brush.set_background(Color::Light_gray);
}

auto Vertical_arrows::mouse_press_event_filter(Widget& /* receiver */,
                                               Mouse::State const& mouse)
    -> bool
{
    if (mouse.button == Mouse::Button::ScrollUp) {
        up_btn.clicked();
        return true;
    }
    else if (mouse.button == Mouse::Button::ScrollDown) {
        down_btn.clicked();
        return true;
    }
    return false;
}

Scroll_btn::Scroll_btn(Glyph_string title) : Push_button{std::move(title)}
{
    this->brush.set_background(Color::Light_gray);
}

auto Scroll_btn::mouse_press_event(Mouse::State const& mouse) -> bool
{
    if (mouse.button == Mouse::Button::ScrollUp)
        scrolled_up();
    else if (mouse.button == Mouse::Button::ScrollDown)
        scrolled_down();
    return Push_button::mouse_press_event(mouse);
}

Horizontal_arrow::Horizontal_arrow(Glyph_string title)
    : arrow{this->make_child<Scroll_btn>(title)}
{
    this->width_policy.fixed(5);
    space.brush.set_background(Color::Black);
}

Offset_control_widget::Offset_control_widget()
{
    this->height_policy.fixed(2);
    left_arrow.arrow.scrolled_up.connect([this]() { right_request(); });
    left_arrow.arrow.scrolled_down.connect([this]() { left_request(); });
    right_arrow.arrow.scrolled_up.connect([this]() { right_request(); });
    right_arrow.arrow.scrolled_down.connect([this]() { left_request(); });
}
}  // namespace gol
