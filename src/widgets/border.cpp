#include <ox/widgets/border.hpp>

#include <iostream>  //temp

#include <ox/core.hpp>
#include <ox/widgets/focus.hpp>

namespace ox {

auto border(BorderInit x) -> Border
{
    return {
        .child = std::move(x.child),
        .box = std::move(x.box),
    };
}

// Child is painted by Application class.
auto paint(Border const& b, Canvas c) -> void { Painter{c}[{0, 0}] << b.box; }

auto resize(Border& b, Area new_size) -> void
{
    b.box.size = new_size;
    b.child.at = {1, 1};
    b.child.size = {
        .width = std::max(0, new_size.width - 2),
        .height = std::max(0, new_size.height - 2),
    };
    resize(b.child, b.child.size);
}

// Forwards Focus
auto focus_in(Border& b) -> void { Focus::set(b.child); }

auto children(Border& b) -> std::span<Widget> { return {&b.child, 1}; }

auto children(Border const& w) -> std::span<Widget const> { return {&w.child, 1}; }

// -------------------------------------------------------------------------------------

auto divider(Glyph line) -> Divider { return {.line = line}; }

}  // namespace ox