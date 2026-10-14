#include <ox/bordered.hpp>

namespace ox {
auto border_content_rect(Area available) -> Rect
{
    validate(available);
    return {{std::min(1, available.width), std::min(1, available.height)},
            {std::max(0, available.width - 2), std::max(0, available.height - 2)}};
}
void paint_border(Canvas canvas, Border const& border)
{
    put(canvas, border.box, border.style);
    auto const size = canvas.size();
    auto const title =
        canvas.subcanvas({std::min(1, size.width), 0},
                         {std::max(0, size.width - 2), std::min(1, size.height)});
    put_label(title, border.label.text, border.label.align, border.label.style);
}
void paint_divider(Canvas canvas,
                   bool vertical,
                   Lines lines,
                   Style style,
                   Label::Options const& label)
{
    fill(canvas, U' ' | style);
    if (vertical) {
        put(canvas, {canvas.size().width / 2, 0},
            shape::VLine{.symbol = lines.vertical}, canvas.size().height, style);
    }
    else {
        put(canvas, {0, canvas.size().height / 2},
            shape::HLine{.symbol = lines.horizontal}, canvas.size().width, style);
    }
    put_label(canvas, label.text, label.align, label.style, vertical);
}
Divider::Options const Divider::init = {};
Divider::Divider(Options x)
    : Widget{FocusPolicy::None, SizePolicy::fixed(1)},
      vertical{x.vertical},
      lines{x.lines},
      style{x.style},
      label{std::move(x.label)}
{}
auto Divider::paint(Canvas canvas) -> Cursor
{
    paint_divider(canvas, vertical, lines, style, label);
    return {};
}
}  // namespace ox
