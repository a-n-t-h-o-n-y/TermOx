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

auto Border::double_line(std::string label) -> Border
{
    return {
        .glyphs = {.corners = {U'╔', U'╗', U'╚', U'╝'}, .walls = {U'═', U'║'}},
        .label{{.text = std::move(label)}},
    };
}

auto Border::double_line(Init state) -> Border
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

auto Border::double_horizontal(std::string label) -> Border
{
    return {
        .glyphs = {.corners = {U'╒', U'╕', U'╘', U'╛'}, .walls = {U'═', U'│'}},
        .label{{.text = std::move(label)}},
    };
}

auto Border::double_horizontal(Init state) -> Border
{
    return {
        .glyphs = {.corners = {U'╒', U'╕', U'╘', U'╛'}, .walls = {U'═', U'│'}},
        .brush = state.brush,
        .label = std::move(state.label),
    };
}

auto Border::double_vertical(std::string label) -> Border
{
    return {
        .glyphs = {.corners = {U'╓', U'╖', U'╙', U'╜'}, .walls = {U'─', U'║'}},
        .label{{.text = std::move(label)}},
    };
}

auto Border::double_vertical(Init state) -> Border
{
    return {
        .glyphs = {.corners = {U'╓', U'╖', U'╙', U'╜'}, .walls = {U'─', U'║'}},
        .brush = state.brush,
        .label = std::move(state.label),
    };
}

// -------------------------------------------------------------------------------------

void Divider::paint(Canvas c) { Painter{c}.fill(line); }

}  // namespace ox