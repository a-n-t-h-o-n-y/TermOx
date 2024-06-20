#include <termox/widgets/border.hpp>

#include <termox/core.hpp>
#include <termox/widgets/focus.hpp>

namespace ox::widgets {

auto paint(Border const& b, Canvas c) -> void
{
    paint(b.child, {.at = c.at + b.child.at, .size = b.child.size});
    Painter{c}[{0, 0}] << b.box;
}

auto resize(Border& b, Area new_size) -> void
{
    b.box.size   = new_size;
    b.child.at   = {1, 1};
    b.child.size = {new_size.width - 2, new_size.height - 2};
    resize(b.child, b.child.size);
}

auto focus_in(Border& b) -> void
{
    Focus::set(b.child);  // Forwards Focus
}

auto timer(Border& b, int id) -> void { timer(b.child, id); }

auto mouse_press(Border& b, Mouse m) -> void
{
    m.at = m.at - b.child.at;
    if (m.at.x >= 0 && m.at.y >= 0 && m.at.x < b.child.size.width &&
        m.at.y < b.child.size.height) {
        mouse_press(b.child, m);
    }
}

auto mouse_release(Border& b, Mouse m) -> void
{
    m.at = m.at - b.child.at;
    if (m.at.x >= 0 && m.at.y >= 0 && m.at.x < b.child.size.width &&
        m.at.y < b.child.size.height) {
        mouse_release(b.child, m);
    }
}

auto mouse_wheel(Border& b, Mouse m) -> void
{
    m.at = m.at - b.child.at;
    if (m.at.x >= 0 && m.at.y >= 0 && m.at.x < b.child.size.width &&
        m.at.y < b.child.size.height) {
        mouse_wheel(b.child, m);
    }
}

auto mouse_move(Border& b, Mouse m) -> void
{
    m.at = m.at - b.child.at;
    if (m.at.x >= 0 && m.at.y >= 0 && m.at.x < b.child.size.width &&
        m.at.y < b.child.size.height) {
        mouse_move(b.child, m);
    }
}

}  // namespace ox::widgets