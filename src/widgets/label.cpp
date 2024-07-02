#include <ox/widgets/label.hpp>

// #include <cstddef>
// #include <string_view>

// #include <ox/core.hpp>

// namespace ox {

// auto label(LabelInit x) -> Label
// {
//     return {
//         .text = std::move(x.text),
//         .align = x.align,
//         .brush = x.brush,
//     };
// }

// void paint(Label const& label, Canvas c)
// {
//     auto& brush = label.brush;
//     Painter{c}.fill(U' ' | bg(brush.background) | fg(brush.foreground));

//     auto const glyphs =
//         std::string_view{label.text}.substr(0, (std::size_t)c.size.width) | brush;

//     auto at = Point{.x = 0, .y = c.size.height / 2};

//     switch (label.align) {
//         case Label::Align::Left: break;
//         case Label::Align::Center:
//             at.x = (c.size.width - (int)glyphs.size()) / 2;
//             break;
//         case Label::Align::Right: at.x = c.size.width - (int)glyphs.size(); break;
//     }

//     Painter{c}[at] << glyphs;
// }

// }  // namespace ox