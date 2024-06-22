#include <termox/widgets/border.hpp>

#include <termox/core.hpp>
#include <termox/widgets/focus.hpp>

namespace ox::widgets {

auto children(Border& b) -> std::span<Widget> { return {&b.child, 1}; }

// Child is painted by Application class.
auto paint(Border const& b, Canvas c) -> void { Painter{c}[{0, 0}] << b.box; }

auto resize(Border& b, Area new_size) -> void
{
    b.box.size = new_size;
    b.child.at = {1, 1};
    b.child.size = {new_size.width - 2, new_size.height - 2};
    resize(b.child, b.child.size);
}

auto focus_in(Border& b) -> void
{
    Focus::set(b.child);  // Forwards Focus
}

}  // namespace ox::widgets