#include <ox/bordered.hpp>

namespace ox {

auto Border::light(std::string label) -> Border
{
    return {
        .box = {.corners = {U'┌', U'┐', U'└', U'┘'}, .walls = {U'─', U'│'}},
        .label{std::move(label)},
    };
}

auto Border::round(std::string label) -> Border
{
    return {
        .box = {.corners = {U'╭', U'╮', U'╰', U'╯'}, .walls = {U'─', U'│'}},
        .label{std::move(label)},
    };
}

auto Border::double_line(std::string label) -> Border
{
    return {
        .box = {.corners = {U'╔', U'╗', U'╚', U'╝'}, .walls = {U'═', U'║'}},
        .label{std::move(label)},
    };
}

auto Border::bold(std::string label) -> Border
{
    return {
        .box = {.corners = {U'┏', U'┓', U'┗', U'┛'}, .walls = {U'━', U'┃'}},
        .label{std::move(label)},
    };
}

auto Border::dashed(std::string label) -> Border
{
    return {
        .box = {.corners = {U'┌', U'┐', U'└', U'┘'}, .walls = {U'╌', U'╎'}},
        .label{std::move(label)},
    };
}

auto Border::dotted(std::string label) -> Border
{
    return {
        .box = {.corners = {U'┌', U'┐', U'└', U'┘'}, .walls = {U'┄', U'┆'}},
        .label{std::move(label)},
    };
}

auto Border::ascii(std::string label) -> Border
{
    return {
        .box = {.corners = {'+', '+', '+', '+'}, .walls = {'-', '|'}},
        .label{std::move(label)},
    };
}

auto Border::double_horizontal(std::string label) -> Border
{
    return {
        .box = {.corners = {U'╒', U'╕', U'╘', U'╛'}, .walls = {U'═', U'│'}},
        .label{std::move(label)},
    };
}

auto Border::double_vertical(std::string label) -> Border
{
    return {
        .box = {.corners = {U'╓', U'╖', U'╙', U'╜'}, .walls = {U'─', U'║'}},
        .label{std::move(label)},
    };
}

// -------------------------------------------------------------------------------------

void Divider::paint(Canvas c) { Painter{c}.fill(line); }

}  // namespace ox