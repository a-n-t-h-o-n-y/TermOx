#include <ox/bordered.hpp>

namespace ox {

auto Border::light(std::string label) -> Border
{
    return {
        .box = shape::Frame::square(),
        .brush = {},
        .label{{.text = std::move(label)}},
    };
}

auto Border::light(Options x) -> Border
{
    return {
        .box = shape::Frame::square(),
        .brush = x.brush,
        .label = std::move(x.label),
    };
}

auto Border::round(std::string label) -> Border
{
    return {
        .box = shape::Frame::round(),
        .brush = {},
        .label{{.text = std::move(label)}},
    };
}

auto Border::round(Options x) -> Border
{
    return {
        .box = shape::Frame::round(),
        .brush = x.brush,
        .label = std::move(x.label),
    };
}

auto Border::twin(std::string label) -> Border
{
    return {
        .box = shape::Frame::twin(),
        .brush = {},
        .label{{.text = std::move(label)}},
    };
}

auto Border::twin(Options x) -> Border
{
    return {
        .box = shape::Frame::twin(),
        .brush = x.brush,
        .label = std::move(x.label),
    };
}

auto Border::bold(std::string label) -> Border
{
    return {
        .box = shape::Frame::bold(),
        .brush = {},
        .label{{.text = std::move(label)}},
    };
}

auto Border::bold(Options x) -> Border
{
    return {
        .box = shape::Frame::bold(),
        .brush = x.brush,
        .label = std::move(x.label),
    };
}

auto Border::dashed(std::string label) -> Border
{
    return {
        .box = {.corners = {U'┌', U'┐', U'└', U'┘'}, .walls = {U'╌', U'╌', U'╎', U'╎'}},
        .brush = {},
        .label{{.text = std::move(label)}},
    };
}

auto Border::dashed(Options x) -> Border
{
    return {
        .box = {.corners = {U'┌', U'┐', U'└', U'┘'}, .walls = {U'╌', U'╌', U'╎', U'╎'}},
        .brush = x.brush,
        .label = std::move(x.label),
    };
}

auto Border::dotted(std::string label) -> Border
{
    return {
        .box = {.corners = {U'┌', U'┐', U'└', U'┘'}, .walls = {U'┄', U'┄', U'┆', U'┆'}},
        .brush = {},
        .label{{.text = std::move(label)}},
    };
}

auto Border::dotted(Options x) -> Border
{
    return {
        .box = {.corners = {U'┌', U'┐', U'└', U'┘'}, .walls = {U'┄', U'┄', U'┆', U'┆'}},
        .brush = x.brush,
        .label = std::move(x.label),
    };
}

auto Border::ascii(std::string label) -> Border
{
    return {
        .box = {.corners = {'+', '+', '+', '+'}, .walls = {'-', '-', '|', '|'}},
        .brush = {},
        .label{{.text = std::move(label)}},
    };
}

auto Border::ascii(Options x) -> Border
{
    return {
        .box = {.corners = {'+', '+', '+', '+'}, .walls = {'-', '-', '|', '|'}},
        .brush = x.brush,
        .label = std::move(x.label),
    };
}

auto Border::twin_horizontal(std::string label) -> Border
{
    return {
        .box = {.corners = {U'╒', U'╕', U'╘', U'╛'}, .walls = {U'═', U'═', U'│', U'│'}},
        .brush = {},
        .label{{.text = std::move(label)}},
    };
}

auto Border::twin_horizontal(Options x) -> Border
{
    return {
        .box = {.corners = {U'╒', U'╕', U'╘', U'╛'}, .walls = {U'═', U'═', U'│', U'│'}},
        .brush = x.brush,
        .label = std::move(x.label),
    };
}

auto Border::twin_vertical(std::string label) -> Border
{
    return {
        .box = {.corners = {U'╓', U'╖', U'╙', U'╜'}, .walls = {U'─', U'─', U'║', U'║'}},
        .brush = {},
        .label{{.text = std::move(label)}},
    };
}

auto Border::twin_vertical(Options x) -> Border
{
    return {
        .box = {.corners = {U'╓', U'╖', U'╙', U'╜'}, .walls = {U'─', U'─', U'║', U'║'}},
        .brush = x.brush,
        .label = std::move(x.label),
    };
}

// -------------------------------------------------------------------------------------

void Divider::paint(Canvas c)
{
    // TODO This isn't a perfect heuristic, a 1 by 1 will not know.
    // only way around this is to have user input or parent layout input to it.
    // This is the same issue with a Horizontal Label, you'll need some input without
    // changing the type. Maybe it is just an enum in Options.

    auto const glyph = this->size.width == 1 ? lines.vertical : lines.horizontal;
    fill(c, glyph | brush);
}

auto Divider::light(std::string label) -> Divider
{
    return Divider::light({
        .label = Label{{.text = std::move(label)}},
    });
}

auto Divider::light(Options x) -> Divider
{
    x.lines = {.vertical = U'│', .horizontal = U'─'};
    return {std::move(x)};
}

auto Divider::bold(std::string label) -> Divider
{
    return Divider::bold({
        .label = Label{{.text = std::move(label)}},
    });
}

auto Divider::bold(Options x) -> Divider
{
    x.lines = {.vertical = U'┃', .horizontal = U'━'};
    return {std::move(x)};
}

auto Divider::twin(std::string label) -> Divider
{
    return Divider::twin({
        .label = Label{{.text = std::move(label)}},
    });
}

auto Divider::twin(Options x) -> Divider
{
    x.lines = {.vertical = U'║', .horizontal = U'═'};
    return {std::move(x)};
}

auto Divider::dashed(std::string label) -> Divider
{
    return Divider::dashed({
        .label = Label{{.text = std::move(label)}},
    });
}

auto Divider::dashed(Options x) -> Divider
{
    x.lines = {.vertical = U'╎', .horizontal = U'╌'};
    return {std::move(x)};
}

auto Divider::dotted(std::string label) -> Divider
{
    return Divider::dotted({
        .label = Label{{.text = std::move(label)}},
    });
}

auto Divider::dotted(Options x) -> Divider
{
    x.lines = {.vertical = U'┆', .horizontal = U'┄'};
    return {std::move(x)};
}

auto Divider::ascii(std::string label) -> Divider
{
    return Divider::ascii({
        .label = Label{{.text = std::move(label)}},
    });
}

auto Divider::ascii(Options x) -> Divider
{
    x.lines = {.vertical = U'-', .horizontal = U'|'};
    return {std::move(x)};
}

auto Divider::empty(std::string label) -> Divider
{
    return Divider::empty({
        .label = Label{{.text = std::move(label)}},
    });
}

auto Divider::empty(Options x) -> Divider
{
    x.lines = {.vertical = U' ', .horizontal = U' '};
    return {std::move(x)};
}

}  // namespace ox