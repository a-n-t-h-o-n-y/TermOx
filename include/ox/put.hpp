#pragma once

#include <array>
#include <string_view>

#include <ox/core/core.hpp>

namespace ox::shape {

struct HLine {
    int length;
    char32_t symbol = U'─';
};

struct VLine {
    int length;
    char32_t symbol = U'│';
};

struct Box {
    /// {top left, top right, bottom left, bottom right}
    std::array<char32_t, 4> corners = square_corners;

    /// {N, S, E, W}
    std::array<char32_t, 4> walls = {U'─', U'─', U'│', U'│'};

    // Color foreground = XColor::Default;
    Area size = {0, 0};

    static constexpr auto square_corners = std::array{U'┌', U'┐', U'└', U'┘'};
    static constexpr auto round_corners = std::array{U'╭', U'╮', U'╰', U'╯'};
};

struct Fill {
    Glyph glyph;
    Area size;
};

struct Frame {
    /// {top left, top right, bottom left, bottom right}
    std::array<char32_t, 4> corners;

    /// {N, S, E, W}
    std::array<char32_t, 4> walls;

    static constexpr auto square() -> Frame
    {
        return {
            .corners = {U'┌', U'┐', U'└', U'┘'},
            .walls = {U'─', U'─', U'│', U'│'},
        };
    }

    static constexpr auto half_square() -> Frame
    {
        return {
            .corners = {U'\0', U'\0', U'\0', U'┘'},
            .walls = {U'\0', U'─', U'│', U'\0'},
        };
    }

    static constexpr auto round() -> Frame
    {
        return {
            .corners = {U'╭', U'╮', U'╰', U'╯'},
            .walls = {U'─', U'─', U'│', U'│'},
        };
    }

    static constexpr auto half_round() -> Frame
    {
        return {
            .corners = {U'\0', U'\0', U'\0', U'╯'},
            .walls = {U'\0', U'─', U'│', U'\0'},
        };
    }

    static constexpr auto bold() -> Frame
    {
        return {
            .corners = {U'┏', U'┓', U'┗', U'┛'},
            .walls = {U'━', U'━', U'┃', U'┃'},
        };
    }

    static constexpr auto half_twin() -> Frame
    {
        return {
            .corners = {U'\0', U'\0', U'\0', U'╝'},
            .walls = {U'\0', U'═', U'║', U'\0'},
        };
    }

    static constexpr auto twin() -> Frame
    {
        return {
            .corners = {U'╔', U'╗', U'╚', U'╝'},
            .walls = {U'═', U'═', U'║', U'║'},
        };
    }

    static constexpr auto half_bold() -> Frame
    {
        return {
            .corners = {U'\0', U'\0', U'\0', U'┛'},
            .walls = {U'\0', U'━', U'┃', U'\0'},
        };
    }

    static constexpr auto corners_square() -> Frame
    {
        return {
            .corners = {U'┌', U'┐', U'└', U'┘'},
            .walls = {U'\0', U'\0', U'\0', U'\0'},
        };
    }

    static constexpr auto corners_round() -> Frame
    {
        return {
            .corners = {U'╭', U'╮', U'╰', U'╯'},
            .walls = {U'\0', U'\0', U'\0', U'\0'},
        };
    }

    static constexpr auto brackets() -> Frame
    {
        return {
            .corners = {U'┌', U'┐', U'└', U'┘'},
            .walls = {U'\0', U'\0', U'│', U'│'},
        };
    }

    static constexpr auto brackets_round() -> Frame
    {
        return {
            .corners = {U'╭', U'╮', U'╰', U'╯'},
            .walls = {U'\0', U'\0', U'│', U'│'},
        };
    }

    static constexpr auto brackets_bold() -> Frame
    {
        return {
            .corners = {U'┏', U'┓', U'┗', U'┛'},
            .walls = {U'\0', U'\0', U'┃', U'┃'},
        };
    }

    static constexpr auto brackets_twin() -> Frame
    {
        return {
            .corners = {U'╔', U'╗', U'╚', U'╝'},
            .walls = {U'\0', U'\0', U'║', U'║'},
        };
    }
};

}  // namespace ox::shape

namespace ox {

/**
 * Put a single Glyph at the given position on the Canvas.
 * @details Does bounds checking and is no-op if \p at is out of Canvas' bounds.
 */
void put(Canvas c, Point at, Glyph const& item);

/**
 * Put a single character type to the Canvas at the given position.
 * @details Uses the default Brush and the given character.
 * @details Does bounds checking and is no-op if \p at is out of Canvas' bounds.
 */
void put(Canvas c, Point at, Character auto item)
{
    put(c, at, Glyph{.symbol = static_cast<char32_t>(item)});
}

void put(Canvas c, Point at, GlyphString auto const& item)
{
    if (at.x < 0 || at.y < 0 || at.y >= c.size.height) { return; }

    for (auto const& g : item) {
        if (at.x >= c.size.width) { break; }
        c[at] = g;
        ++at.x;
    }
}

void put(Canvas c, Point at, std::string_view element);

void put(Canvas c, Point at, std::u32string_view element);

/**
 * Paint the given HLine object to the screen.
 *
 * @param c The Canvas to Paint on.
 * @param at The leftmost point of the line, where painting begins.
 * @param item The HLine to paint.
 */
void put(Canvas c, Point at, shape::HLine item);

/**
 * Paint the given HLine object to the screen.
 *
 * @param c The Canvas to Paint on.
 * @param at The leftmost point of the line, where painting begins.
 * @param item The HLine to paint.
 * @param foreground The fg color to assign each Cell the line is painted to.
 */
void put(Canvas c, Point at, shape::HLine item, Color foreground);

/**
 * Paint the given HLine object to the screen.
 *
 * @param c The Canvas to Paint on.
 * @param at The leftmost point of the line, where painting begins.
 * @param item The HLine to paint.
 * @param foreground The Brush to assign each Cell the line is painted to.
 */
void put(Canvas c, Point at, shape::HLine item, Brush const& brush);

void put(Canvas c, Point at, shape::VLine item);
void put(Canvas c, Point at, shape::VLine item, Color foreground);
void put(Canvas c, Point at, shape::VLine item, Brush const& brush);

void put(Canvas c, Point at, shape::Box const& item);
void put(Canvas c, Point at, shape::Box const& item, Color foreground);
void put(Canvas c, Point at, shape::Box const& item, Brush const& brush);

void put(Canvas c, Point at, shape::Fill const& item);

void put(Canvas c, shape::Frame const& item);
void put(Canvas c, shape::Frame const& item, Color foreground);
void put(Canvas c, shape::Frame const& item, Brush const& brush);

void clear(Canvas c);

}  // namespace ox
