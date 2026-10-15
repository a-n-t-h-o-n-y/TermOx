#include <ox/bordered.hpp>
#include <ox/rule.hpp>

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
    : Widget{FocusPolicy::None, x.vertical ? SizePolicy::fixed(1) : SizePolicy::flex(),
             x.vertical ? SizePolicy::flex() : SizePolicy::fixed(1)},
      vertical{x.vertical},
      lines{x.lines},
      style{x.style},
      label{std::move(x.label)}
{}
auto Divider::paint(Canvas canvas) -> Cursor
{
    auto caption = label;
    caption.style = appearance(caption.style, Surface::Muted);
    auto strokes = lines;
    if (theme().styled && theme().ascii) strokes = Lines::ascii();
    paint_divider(canvas, vertical, strokes, appearance(style, Surface::Border),
                  caption);
    auto symbol = vertical ? strokes.vertical : strokes.horizontal;
    if (theme().styled && !theme().ascii &&
        (symbol == U'─' || symbol == U'│' || symbol == U'━' || symbol == U'┃')) {
        auto view = vertical ? canvas.subcanvas({canvas.size().width / 2, 0},
                                                {1, canvas.size().height})
                             : canvas.subcanvas({0, canvas.size().height / 2},
                                                {canvas.size().width, 1});
        auto bold = symbol == U'━' || symbol == U'┃';
        auto n = vertical ? view.size().height : view.size().width;
        auto line = appearance(style, Surface::Border);
        paint_rule(view, 0, bold ? n : 0, line, line, vertical);
        put_label(canvas, caption.text, caption.align, caption.style, vertical);
    }
    return {};
}
}  // namespace ox
