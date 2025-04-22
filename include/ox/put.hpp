#pragma once

#include <array>
#include <string_view>

#include <ox/core/core.hpp>

namespace ox::shape {

struct HLine {
    char32_t symbol = U'─';
};

struct VLine {
    char32_t symbol = U'│';
};

struct Box {
    /// {top left, top right, bottom left, bottom right}
    std::array<char32_t, 4> corners;

    /// {N, S, E, W}
    std::array<char32_t, 4> walls;

    [[nodiscard]] static constexpr auto square() -> Box
    {
        return {
            .corners = {U'┌', U'┐', U'└', U'┘'},
            .walls = {U'─', U'─', U'│', U'│'},
        };
    }

    [[nodiscard]] static constexpr auto half_square() -> Box
    {
        return {
            .corners = {U'\0', U'\0', U'\0', U'┘'},
            .walls = {U'\0', U'─', U'│', U'\0'},
        };
    }

    [[nodiscard]] static constexpr auto round() -> Box
    {
        return {
            .corners = {U'╭', U'╮', U'╰', U'╯'},
            .walls = {U'─', U'─', U'│', U'│'},
        };
    }

    [[nodiscard]] static constexpr auto half_round() -> Box
    {
        return {
            .corners = {U'\0', U'\0', U'\0', U'╯'},
            .walls = {U'\0', U'─', U'│', U'\0'},
        };
    }

    [[nodiscard]] static constexpr auto bold() -> Box
    {
        return {
            .corners = {U'┏', U'┓', U'┗', U'┛'},
            .walls = {U'━', U'━', U'┃', U'┃'},
        };
    }

    [[nodiscard]] static constexpr auto dashed() -> Box
    {
        return {
            .corners = {U'┌', U'┐', U'└', U'┘'},
            .walls = {U'╌', U'╌', U'╎', U'╎'},
        };
    }

    [[nodiscard]] static constexpr auto dotted() -> Box
    {
        return {
            .corners = {U'┌', U'┐', U'└', U'┘'},
            .walls = {U'┄', U'┄', U'┆', U'┆'},
        };
    }

    [[nodiscard]] static constexpr auto ascii() -> Box
    {
        return {
            .corners = {'+', '+', '+', '+'},
            .walls = {'-', '-', '|', '|'},
        };
    }

    [[nodiscard]] static constexpr auto half_twin() -> Box
    {
        return {
            .corners = {U'\0', U'\0', U'\0', U'╝'},
            .walls = {U'\0', U'═', U'║', U'\0'},
        };
    }

    [[nodiscard]] static constexpr auto twin() -> Box
    {
        return {
            .corners = {U'╔', U'╗', U'╚', U'╝'},
            .walls = {U'═', U'═', U'║', U'║'},
        };
    }

    [[nodiscard]] static constexpr auto half_bold() -> Box
    {
        return {
            .corners = {U'\0', U'\0', U'\0', U'┛'},
            .walls = {U'\0', U'━', U'┃', U'\0'},
        };
    }

    [[nodiscard]] static constexpr auto corners_square() -> Box
    {
        return {
            .corners = {U'┌', U'┐', U'└', U'┘'},
            .walls = {U'\0', U'\0', U'\0', U'\0'},
        };
    }

    [[nodiscard]] static constexpr auto corners_round() -> Box
    {
        return {
            .corners = {U'╭', U'╮', U'╰', U'╯'},
            .walls = {U'\0', U'\0', U'\0', U'\0'},
        };
    }

    [[nodiscard]] static constexpr auto brackets() -> Box
    {
        return {
            .corners = {U'┌', U'┐', U'└', U'┘'},
            .walls = {U'\0', U'\0', U'│', U'│'},
        };
    }

    [[nodiscard]] static constexpr auto brackets_round() -> Box
    {
        return {
            .corners = {U'╭', U'╮', U'╰', U'╯'},
            .walls = {U'\0', U'\0', U'│', U'│'},
        };
    }

    [[nodiscard]] static constexpr auto brackets_bold() -> Box
    {
        return {
            .corners = {U'┏', U'┓', U'┗', U'┛'},
            .walls = {U'\0', U'\0', U'┃', U'┃'},
        };
    }

    [[nodiscard]] static constexpr auto brackets_twin() -> Box
    {
        return {
            .corners = {U'╔', U'╗', U'╚', U'╝'},
            .walls = {U'\0', U'\0', U'║', U'║'},
        };
    }

    [[nodiscard]] static constexpr auto twin_horizontal() -> Box
    {
        return {
            .corners = {U'╒', U'╕', U'╘', U'╛'},
            .walls = {U'═', U'═', U'│', U'│'},
        };
    }

    [[nodiscard]] static constexpr auto twin_vertical() -> Box
    {
        return {
            .corners = {U'╓', U'╖', U'╙', U'╜'},
            .walls = {U'─', U'─', U'║', U'║'},
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
 * @param length The number of cells to paint.
 */
void put(Canvas c, Point at, shape::HLine item, int length);

/**
 * Paint the given HLine object to the screen.
 *
 * @param c The Canvas to Paint on.
 * @param at The leftmost point of the line, where painting begins.
 * @param item The HLine to paint.
 * @param length The number of cells to paint.
 * @param foreground The fg color to assign each Cell the line is painted to.
 */
void put(Canvas c, Point at, shape::HLine item, int length, Color foreground);

/**
 * Paint the given HLine object to the screen.
 *
 * @param c The Canvas to Paint on.
 * @param at The leftmost point of the line, where painting begins.
 * @param item The HLine to paint.
 * @param length The number of cells to paint.
 * @param foreground The Brush to assign each Cell the line is painted to.
 */
void put(Canvas c, Point at, shape::HLine item, int length, Brush const& brush);

void put(Canvas c, Point at, shape::VLine item, int length);
void put(Canvas c, Point at, shape::VLine item, int length, Color foreground);
void put(Canvas c, Point at, shape::VLine item, int length, Brush const& brush);

void put(Canvas c, Point at, shape::Box const& item, Area size);
void put(Canvas c, Point at, shape::Box const& item, Area size, Color foreground);
void put(Canvas c, Point at, shape::Box const& item, Area size, Brush const& brush);

void put(Canvas c, shape::Box const& item);
void put(Canvas c, shape::Box const& item, Color foreground);
void put(Canvas c, shape::Box const& item, Brush const& brush);

void fill(Canvas c, Glyph g);
void fill(Canvas c, Brush b);
void fill(Canvas c, char32_t ch);
void fill(Canvas c, ColorBG bg);
void fill(Canvas c, ColorFG fg);
void fill(Canvas c, Traits ts);

void clear(Canvas c);

}  // namespace ox
