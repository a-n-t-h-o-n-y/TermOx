#include <ox/bordered.hpp>

namespace ox {

auto Border::light(std::string label) -> Border
{
    return {
        .glyphs = {.corners = {U'┌', U'┐', U'└', U'┘'}, .walls = {U'─', U'│'}},
        .label{{.text = std::move(label)}},
    };
}

auto Border::light(Init state) -> Border
{
    return {
        .glyphs = {.corners = {U'┌', U'┐', U'└', U'┘'}, .walls = {U'─', U'│'}},
        .brush = state.brush,
        .label = std::move(state.label),
    };
}

auto Border::round(std::string label) -> Border
{
    return {
        .glyphs = {.corners = {U'╭', U'╮', U'╰', U'╯'}, .walls = {U'─', U'│'}},
        .label{{.text = std::move(label)}},
    };
}

auto Border::round(Init state) -> Border
{
    return {
        .glyphs = {.corners = {U'╭', U'╮', U'╰', U'╯'}, .walls = {U'─', U'│'}},
        .brush = state.brush,
        .label = std::move(state.label),
    };
}

auto Border::twin(std::string label) -> Border
{
    return {
        .glyphs = {.corners = {U'╔', U'╗', U'╚', U'╝'}, .walls = {U'═', U'║'}},
        .label{{.text = std::move(label)}},
    };
}

auto Border::twin(Init state) -> Border
{
    return {
        .glyphs = {.corners = {U'╔', U'╗', U'╚', U'╝'}, .walls = {U'═', U'║'}},
        .brush = state.brush,
        .label = std::move(state.label),
    };
}

auto Border::bold(std::string label) -> Border
{
    return {
        .glyphs = {.corners = {U'┏', U'┓', U'┗', U'┛'}, .walls = {U'━', U'┃'}},
        .label{{.text = std::move(label)}},
    };
}

auto Border::bold(Init state) -> Border
{
    return {
        .glyphs = {.corners = {U'┏', U'┓', U'┗', U'┛'}, .walls = {U'━', U'┃'}},
        .brush = state.brush,
        .label = std::move(state.label),
    };
}

auto Border::dashed(std::string label) -> Border
{
    return {
        .glyphs = {.corners = {U'┌', U'┐', U'└', U'┘'}, .walls = {U'╌', U'╎'}},
        .label{{.text = std::move(label)}},
    };
}

auto Border::dashed(Init state) -> Border
{
    return {
        .glyphs = {.corners = {U'┌', U'┐', U'└', U'┘'}, .walls = {U'╌', U'╎'}},
        .brush = state.brush,
        .label = std::move(state.label),
    };
}

auto Border::dotted(std::string label) -> Border
{
    return {
        .glyphs = {.corners = {U'┌', U'┐', U'└', U'┘'}, .walls = {U'┄', U'┆'}},
        .label{{.text = std::move(label)}},
    };
}

auto Border::dotted(Init state) -> Border
{
    return {
        .glyphs = {.corners = {U'┌', U'┐', U'└', U'┘'}, .walls = {U'┄', U'┆'}},
        .brush = state.brush,
        .label = std::move(state.label),
    };
}

auto Border::ascii(std::string label) -> Border
{
    return {
        .glyphs = {.corners = {'+', '+', '+', '+'}, .walls = {'-', '|'}},
        .label{{.text = std::move(label)}},
    };
}

auto Border::ascii(Init state) -> Border
{
    return {
        .glyphs = {.corners = {'+', '+', '+', '+'}, .walls = {'-', '|'}},
        .brush = state.brush,
        .label = std::move(state.label),
    };
}

auto Border::twin_horizontal(std::string label) -> Border
{
    return {
        .glyphs = {.corners = {U'╒', U'╕', U'╘', U'╛'}, .walls = {U'═', U'│'}},
        .label{{.text = std::move(label)}},
    };
}

auto Border::twin_horizontal(Init state) -> Border
{
    return {
        .glyphs = {.corners = {U'╒', U'╕', U'╘', U'╛'}, .walls = {U'═', U'│'}},
        .brush = state.brush,
        .label = std::move(state.label),
    };
}

auto Border::twin_vertical(std::string label) -> Border
{
    return {
        .glyphs = {.corners = {U'╓', U'╖', U'╙', U'╜'}, .walls = {U'─', U'║'}},
        .label{{.text = std::move(label)}},
    };
}

auto Border::twin_vertical(Init state) -> Border
{
    return {
        .glyphs = {.corners = {U'╓', U'╖', U'╙', U'╜'}, .walls = {U'─', U'║'}},
        .brush = state.brush,
        .label = std::move(state.label),
    };
}

// -------------------------------------------------------------------------------------

void Divider::paint(Canvas c)
{
    // TODO This isn't a perfect heuristic, a 1 by 1 will not know.
    // only way around this is to have user input or parent layout input to it.
    // This is the same issue with a Horizontal Label, you'll need some input without
    // changing the type. Maybe it is just an enum in Init.
    if (this->size.width == 1) {
        Painter{c}.fill(lines.vertical | brush);
    }
    else {
        Painter{c}.fill(lines.horizontal | brush);
    }
}

auto Divider::light(std::string label) -> Divider
{
    return Divider::light({
        .label = Label{{.text = std::move(label)}},
    });
}

auto Divider::light(Init state) -> Divider
{
    state.lines = {.vertical = U'│', .horizontal = U'─'};
    return {std::move(state)};
}

auto Divider::bold(std::string label) -> Divider
{
    return Divider::bold({
        .label = Label{{.text = std::move(label)}},
    });
}

auto Divider::bold(Init state) -> Divider
{
    state.lines = {.vertical = U'┃', .horizontal = U'━'};
    return {std::move(state)};
}

auto Divider::twin(std::string label) -> Divider
{
    return Divider::twin({
        .label = Label{{.text = std::move(label)}},
    });
}

auto Divider::twin(Init state) -> Divider
{
    state.lines = {.vertical = U'║', .horizontal = U'═'};
    return {std::move(state)};
}

auto Divider::dashed(std::string label) -> Divider
{
    return Divider::dashed({
        .label = Label{{.text = std::move(label)}},
    });
}

auto Divider::dashed(Init state) -> Divider
{
    state.lines = {.vertical = U'╎', .horizontal = U'╌'};
    return {std::move(state)};
}

auto Divider::dotted(std::string label) -> Divider
{
    return Divider::dotted({
        .label = Label{{.text = std::move(label)}},
    });
}

auto Divider::dotted(Init state) -> Divider
{
    state.lines = {.vertical = U'┆', .horizontal = U'┄'};
    return {std::move(state)};
}

auto Divider::ascii(std::string label) -> Divider
{
    return Divider::ascii({
        .label = Label{{.text = std::move(label)}},
    });
}

auto Divider::ascii(Init state) -> Divider
{
    state.lines = {.vertical = U'-', .horizontal = U'|'};
    return {std::move(state)};
}

auto Divider::empty(std::string label) -> Divider
{
    return Divider::empty({
        .label = Label{{.text = std::move(label)}},
    });
}

auto Divider::empty(Init state) -> Divider
{
    state.lines = {.vertical = U' ', .horizontal = U' '};
    return {std::move(state)};
}

}  // namespace ox