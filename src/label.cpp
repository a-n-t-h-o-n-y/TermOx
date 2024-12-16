#include <ox/label.hpp>

#include <cstddef>
#include <string_view>

#include <ox/core/core.hpp>

namespace ox {

Label::Init const Label::init = {};

Label::Label(Init state)
    : Widget{FocusPolicy::None, SizePolicy::flex()},
      text{std::move(state.text)},
      align{state.align},
      brush{state.brush}
{}

void Label::paint(Canvas c)
{
    Painter{c}.fill(U' ' | bg(brush.background));

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