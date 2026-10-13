#include <ox/bordered.hpp>

namespace ox {

Divider::Divider(Options x)
    : Widget{FocusPolicy::None, SizePolicy::fixed(1)},
      lines{std::move(x.lines)},
      style{std::move(x.style)},
      label{std::move(x.label)}
{}

void Divider::paint(Canvas c)
{
    // TODO This isn't a perfect heuristic, a 1 by 1 will not know.
    // only way around this is to have user input or parent layout input to it.
    // This is the same issue with a Horizontal Label, you'll need some input without
    // changing the type. Maybe it is just an enum in Options.

    auto const glyph = this->size.width == 1 ? lines.vertical : lines.horizontal;
    fill(c, glyph | style);
}

}  // namespace ox