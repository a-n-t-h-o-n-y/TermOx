/** @file
 * Clipped drawing of glyphs, text, lines, boxes, and cell styles.
 */
#pragma once

#include <array>
#include <string_view>

#include <ox/core/canvas.hpp>
#include <ox/core/text.hpp>

namespace ox::shape {

/** Horizontal repeated one-column scalar; a null symbol leaves cells untouched. */
struct HLine {
    /** Printable one-column stroke scalar; null skips drawing. */
    char32_t symbol = U'─';
};

/** Vertical repeated one-column scalar; a null symbol leaves cells untouched. */
struct VLine {
    /** Printable one-column stroke scalar; null skips drawing. */
    char32_t symbol = U'│';
};

/** Outline glyphs for corners and walls; null scalars omit individual segments, and
 * non-null scalars must occupy one column.
 */
struct Box {
    /// {top left, top right, bottom left, bottom right}
    std::array<char32_t, 4> corners;

    /// {N, S, E, W}
    std::array<char32_t, 4> walls;

    /** Square single-line outline. */
    [[nodiscard]] static constexpr auto square() -> Box
    {
        return {
            .corners = {U'┌', U'┐', U'└', U'┘'},
            .walls = {U'─', U'─', U'│', U'│'},
        };
    }

    /** Single-line bottom and right edges with their lower-right corner. */
    [[nodiscard]] static constexpr auto half_square() -> Box
    {
        return {
            .corners = {U'\0', U'\0', U'\0', U'┘'},
            .walls = {U'\0', U'─', U'│', U'\0'},
        };
    }

    /** Single-line outline with rounded corners. */
    [[nodiscard]] static constexpr auto round() -> Box
    {
        return {
            .corners = {U'╭', U'╮', U'╰', U'╯'},
            .walls = {U'─', U'─', U'│', U'│'},
        };
    }

    /** Bottom and right single-line edges with a rounded lower-right corner. */
    [[nodiscard]] static constexpr auto half_round() -> Box
    {
        return {
            .corners = {U'\0', U'\0', U'\0', U'╯'},
            .walls = {U'\0', U'─', U'│', U'\0'},
        };
    }

    /** Heavy square outline. */
    [[nodiscard]] static constexpr auto bold() -> Box
    {
        return {
            .corners = {U'┏', U'┓', U'┗', U'┛'},
            .walls = {U'━', U'━', U'┃', U'┃'},
        };
    }

    /** Dashed single-line walls with square corners. */
    [[nodiscard]] static constexpr auto dashed() -> Box
    {
        return {
            .corners = {U'┌', U'┐', U'└', U'┘'},
            .walls = {U'╌', U'╌', U'╎', U'╎'},
        };
    }

    /** Dotted single-line walls with square corners. */
    [[nodiscard]] static constexpr auto dotted() -> Box
    {
        return {
            .corners = {U'┌', U'┐', U'└', U'┘'},
            .walls = {U'┄', U'┄', U'┆', U'┆'},
        };
    }

    /** ASCII plus-sign corners with hyphen and vertical-bar walls. */
    [[nodiscard]] static constexpr auto ascii() -> Box
    {
        return {
            .corners = {'+', '+', '+', '+'},
            .walls = {'-', '-', '|', '|'},
        };
    }

    /** Double-line bottom and right edges with their lower-right corner. */
    [[nodiscard]] static constexpr auto half_twin() -> Box
    {
        return {
            .corners = {U'\0', U'\0', U'\0', U'╝'},
            .walls = {U'\0', U'═', U'║', U'\0'},
        };
    }

    /** Double-line square outline. */
    [[nodiscard]] static constexpr auto twin() -> Box
    {
        return {
            .corners = {U'╔', U'╗', U'╚', U'╝'},
            .walls = {U'═', U'═', U'║', U'║'},
        };
    }

    /** Heavy bottom and right edges with their lower-right corner. */
    [[nodiscard]] static constexpr auto half_bold() -> Box
    {
        return {
            .corners = {U'\0', U'\0', U'\0', U'┛'},
            .walls = {U'\0', U'━', U'┃', U'\0'},
        };
    }

    /** Square corners only; leave all wall cells untouched. */
    [[nodiscard]] static constexpr auto corners_square() -> Box
    {
        return {
            .corners = {U'┌', U'┐', U'└', U'┘'},
            .walls = {U'\0', U'\0', U'\0', U'\0'},
        };
    }

    /** Rounded corners only; leave all wall cells untouched. */
    [[nodiscard]] static constexpr auto corners_round() -> Box
    {
        return {
            .corners = {U'╭', U'╮', U'╰', U'╯'},
            .walls = {U'\0', U'\0', U'\0', U'\0'},
        };
    }

    /** Single-line vertical sides and square corners, leaving horizontal walls untouched. */
    [[nodiscard]] static constexpr auto brackets() -> Box
    {
        return {
            .corners = {U'┌', U'┐', U'└', U'┘'},
            .walls = {U'\0', U'\0', U'│', U'│'},
        };
    }

    /** Single-line vertical sides with rounded corners. */
    [[nodiscard]] static constexpr auto brackets_round() -> Box
    {
        return {
            .corners = {U'╭', U'╮', U'╰', U'╯'},
            .walls = {U'\0', U'\0', U'│', U'│'},
        };
    }

    /** Heavy vertical sides and corners. */
    [[nodiscard]] static constexpr auto brackets_bold() -> Box
    {
        return {
            .corners = {U'┏', U'┓', U'┗', U'┛'},
            .walls = {U'\0', U'\0', U'┃', U'┃'},
        };
    }

    /** Double-line vertical sides and corners. */
    [[nodiscard]] static constexpr auto brackets_twin() -> Box
    {
        return {
            .corners = {U'╔', U'╗', U'╚', U'╝'},
            .walls = {U'\0', U'\0', U'║', U'║'},
        };
    }

    /** Double horizontal walls joined to single vertical walls. */
    [[nodiscard]] static constexpr auto twin_horizontal() -> Box
    {
        return {
            .corners = {U'╒', U'╕', U'╘', U'╛'},
            .walls = {U'═', U'═', U'│', U'│'},
        };
    }

    /** Double vertical walls joined to single horizontal walls. */
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
 * @details Uses the default Style and the given character.
 * @details Does bounds checking and is no-op if \p at is out of Canvas' bounds.
 */
void put(Canvas c, Point at, Character auto item) { put(c, at, item | Style{}); }

/** Glyph colors inherit defaults only when unspecified; the patch applies last. */
void put(Canvas c,
         Point at,
         std::span<Glyph const> item,
         StylePatch patch = {},
         Style defaults = {});

/** Paint a glyph range using default colors only for unspecified channels, then
 * apply patch. Noncontiguous ranges are copied into temporary glyph storage.
 * Validate complete graphemes before drawing; never split a clipped grapheme.
 */
void put(Canvas c,
         Point at,
         GlyphRange auto const& item,
         StylePatch patch = {},
         Style defaults = {})
{
    if constexpr (std::ranges::contiguous_range<decltype(item)> &&
                  std::ranges::sized_range<decltype(item)>) {
        put(c, at,
            std::span<Glyph const>{std::ranges::data(item), std::ranges::size(item)},
            patch, defaults);
    }
    else {
        auto glyphs = std::vector<Glyph>{};
        for (auto const& glyph : item) {
            glyphs.push_back(glyph);
        }
        put(c, at, std::span<Glyph const>{glyphs}, patch, defaults);
    }
}

/** Paint validated UTF-8 graphemes horizontally with one style; clipped graphemes are
 * omitted whole and text never wraps.
 */
void put(Canvas c, Point at, std::string_view element, Style style = {});

/** Paint validated UTF-32 graphemes horizontally with one style; clipped graphemes are
 * omitted whole and text never wraps.
 */
void put(Canvas c, Point at, std::u32string_view element, Style style = {});

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
 * @param style The Style to assign each Cell the line is painted to.
 */
void put(Canvas c, Point at, shape::HLine item, int length, Style const& style);

/** Paint a clipped vertical stroke with existing cell styles; reject negative length or
 * invalid stroke width.
 */
void put(Canvas c, Point at, shape::VLine item, int length);
/** Paint a clipped vertical stroke, replacing foreground only and preserving other cell
 * attributes.
 */
void put(Canvas c, Point at, shape::VLine item, int length, Color foreground);
/** Paint a clipped vertical stroke with the supplied complete style; validate before
 * drawing.
 */
void put(Canvas c, Point at, shape::VLine item, int length, Style const& style);

/** Paint a clipped outline at a local origin, preserving existing cell styles; reject
 * invalid dimensions or glyphs.
 */
void put(Canvas c, Point at, shape::Box const& item, Area size);
/** Paint a clipped outline, replacing stroke foregrounds while preserving other cell
 * attributes.
 */
void put(Canvas c, Point at, shape::Box const& item, Area size, Color foreground);
/** Paint a clipped outline with one complete validated style; leave interior cells
 * untouched.
 */
void put(Canvas c, Point at, shape::Box const& item, Area size, Style const& style);

/** Draw an outline around the canvas's full logical extent with existing cell styles. */
void put(Canvas c, shape::Box const& item);
/** Draw an outline around the canvas's full logical extent, replacing stroke foregrounds. */
void put(Canvas c, shape::Box const& item, Color foreground);
/** Draw an outline around the canvas's full logical extent with the supplied style. */
void put(Canvas c, shape::Box const& item, Style const& style);

/** Tile the visible clip with a validated glyph and its style, preserving complete
 * grapheme footprints.
 */
void fill(Canvas c, Glyph g);
/** Replace styles of fully visible graphemes while preserving their text and width. */
void fill(Canvas c, Style b);
/** Tile visible cells with a validated scalar while preserving existing cell styles. */
void fill(Canvas c, char32_t ch);
/** Change only the background color of fully visible graphemes. */
void fill(Canvas c, ColorBG bg);
/** Change only the foreground color of fully visible graphemes. */
void fill(Canvas c, ColorFG fg);
/** Replace traits of fully visible graphemes, retaining text and colors. */
void fill(Canvas c, Traits ts);

/** Fill the visible area outside a parent-local content rectangle. */
void fill_outside(Canvas canvas, Rect content, Glyph glyph = {});

/** Fill the visible clip with default-styled spaces; cells outside the clip remain
 * unchanged.
 */
void clear(Canvas c);

/** Draw one cached display row, preserving complete glyphs and selection styling. */
void put_text_line(Canvas canvas,
                   Point at,
                   TextLayout const& layout,
                   std::size_t row,
                   Style normal,
                   Style selected,
                   std::pair<std::size_t, std::size_t> selection = {0, 0});

}  // namespace ox
