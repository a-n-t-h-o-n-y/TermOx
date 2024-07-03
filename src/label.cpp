#include <ox/label.hpp>

// #include <cstddef>
// #include <string_view>

#include <ox/core/core.hpp>

namespace ox {

// auto label(LabelInit x) -> Label
// {
//     return {
//         .text = std::move(x.text),
//         .align = x.align,
//         .brush = x.brush,
//     };
// }

void Label::paint(Canvas c) const
{
    Painter{c}.fill(U' ' | bg(brush.background) | fg(brush.foreground));

    auto const glyphs =
        std::string_view{text}.substr(0, (std::size_t)c.size.width) | brush;

    auto at = Point{.x = 0, .y = c.size.height / 2};

    switch (align) {
        case Align::Left: break;
        case Align::Center: at.x = (c.size.width - (int)glyphs.size()) / 2; break;
        case Align::Right: at.x = c.size.width - (int)glyphs.size(); break;
    }

    Painter{c}[at] << glyphs;
}

}  // namespace ox