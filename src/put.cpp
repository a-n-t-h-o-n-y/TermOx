#include <ox/put.hpp>

#include <string_view>

#include <ox/core/core.hpp>

namespace {

/**
 *  Return true if \p at is within the bounds of \p area.
 */
[[nodiscard]]
auto in_bounds(ox::Point at, ox::Area area) -> bool
{
    return at.x >= 0 && at.y >= 0 && at.x < area.width && at.y < area.height;
}

}  // namespace

namespace ox {

void put(Canvas c, Point at, Glyph const& item)
{
    if (in_bounds(at, c.size)) { c[at] = item; }
}

void put(Canvas c, Point at, std::string_view item)
{
    put(c, at, esc::detail::utf8_to_glyphs(item));
}

void put(Canvas c, Point at, std::u32string_view item)
{
    put(c, at, esc::detail::utf32_to_glyphs(item));
}

void put(Canvas c, Point at, shape::HLine item)
{
    if (at.x < 0 || at.y < 0 || at.y >= c.size.height || item.symbol == U'\0') {
        return;
    }

    auto const end = std::min(at.x + item.length, c.size.width);
    while (at.x < end) {
        c[at].symbol = item.symbol;
        ++at.x;
    }
}

void put(Canvas c, Point at, shape::HLine item, Color foreground)
{
    if (at.x < 0 || at.y < 0 || at.y >= c.size.height || item.symbol == U'\0') {
        return;
    }

    auto const end = std::min(at.x + item.length, c.size.width);
    while (at.x < end) {
        c[at].symbol = item.symbol;
        c[at].brush.foreground = foreground;
        ++at.x;
    }
}

void put(Canvas c, Point at, shape::HLine item, Brush const& brush)
{
    if (at.x < 0 || at.y < 0 || at.y >= c.size.height || item.symbol == U'\0') {
        return;
    }

    auto const end = std::min(at.x + item.length, c.size.width);
    while (at.x < end) {
        c[at].symbol = item.symbol;
        c[at].brush = brush;
        ++at.x;
    }
}

void put(Canvas c, Point at, shape::VLine item)
{
    if (at.x < 0 || at.y < 0 || at.x >= c.size.width || item.symbol == U'\0') {
        return;
    }

    auto const end = std::min(at.y + item.length, c.size.height);
    while (at.y < end) {
        c[at].symbol = item.symbol;
        ++at.y;
    }
}

void put(Canvas c, Point at, shape::VLine item, Color foreground)
{
    if (at.x < 0 || at.y < 0 || at.x >= c.size.width || item.symbol == U'\0') {
        return;
    }

    auto const end = std::min(at.y + item.length, c.size.height);
    while (at.y < end) {
        c[at].symbol = item.symbol;
        c[at].brush.foreground = foreground;
        ++at.y;
    }
}

void put(Canvas c, Point at, shape::VLine item, Brush const& brush)
{
    if (at.x < 0 || at.y < 0 || at.x >= c.size.width || item.symbol == U'\0') {
        return;
    }

    auto const end = std::min(at.y + item.length, c.size.height);
    while (at.y < end) {
        c[at].symbol = item.symbol;
        c[at].brush = brush;
        ++at.y;
    }
}

void put(Canvas c, Point at, shape::Box const& item)
{
    // One Past Bottom Right Corner
    auto const end = Point{
        .x = std::min(at.x + item.size.width, c.size.width),
        .y = std::min(at.y + item.size.height, c.size.height),
    };

    // Horizontal
    put(c, {at.x + 1, at.y},
        shape::HLine{
            .length = item.size.width - 2,
            .symbol = item.walls[0],
        });
    put(c, {at.x + 1, end.y - 1},
        shape::HLine{
            .length = item.size.width - 2,
            .symbol = item.walls[1],
        });

    // Vertical
    put(c, {at.x, at.y + 1},
        shape::VLine{
            .length = item.size.height - 2,
            .symbol = item.walls[3],
        });
    put(c, {end.x - 1, at.y + 1},
        shape::VLine{
            .length = item.size.height - 2,
            .symbol = item.walls[2],
        });

    // Corners
    if (item.corners[0] != U'\0' && in_bounds(at, c.size)) {
        c[at].symbol = item.corners[0];
    }

    if (item.corners[1] != U'\0') {
        auto const pt = Point{end.x - 1, at.y};
        if (in_bounds(pt, c.size)) { c[pt].symbol = item.corners[1]; }
    }

    if (item.corners[2] != U'\0') {
        auto const pt = Point{at.x, end.y - 1};
        if (in_bounds(pt, c.size)) { c[pt].symbol = item.corners[2]; }
    }

    if (item.corners[3] != U'\0') {
        auto const pt = Point{end.x - 1, end.y - 1};
        if (in_bounds(pt, c.size)) { c[pt].symbol = item.corners[3]; }
    }
}

void put(Canvas c, Point at, shape::Box const& item, Color foreground)
{
    // One Past Bottom Right Corner
    auto const end = Point{
        .x = std::min(at.x + item.size.width, c.size.width),
        .y = std::min(at.y + item.size.height, c.size.height),
    };

    // Horizontal
    put(c, {at.x + 1, at.y},
        shape::HLine{
            .length = item.size.width - 2,
            .symbol = item.walls[0],
        },
        foreground);
    put(c, {at.x + 1, end.y - 1},
        shape::HLine{
            .length = item.size.width - 2,
            .symbol = item.walls[1],
        },
        foreground);

    // Vertical
    put(c, {at.x, at.y + 1},
        shape::VLine{
            .length = item.size.height - 2,
            .symbol = item.walls[3],
        },
        foreground);
    put(c, {end.x - 1, at.y + 1},
        shape::VLine{
            .length = item.size.height - 2,
            .symbol = item.walls[2],
        },
        foreground);

    auto const in_bounds = [](Point at, Area area) {
        return at.x >= 0 && at.y >= 0 && at.x < area.width && at.y < area.height;
    };

    // Corners
    if (item.corners[0] != U'\0' && in_bounds(at, c.size)) {
        c[at].symbol = item.corners[0];
        c[at].brush.foreground = foreground;
    }

    if (item.corners[1] != U'\0') {
        auto const pt = Point{end.x - 1, at.y};
        if (in_bounds(pt, c.size)) {
            c[pt].symbol = item.corners[1];
            c[pt].brush.foreground = foreground;
        }
    }

    if (item.corners[2] != U'\0') {
        auto const pt = Point{at.x, end.y - 1};
        if (in_bounds(pt, c.size)) {
            c[pt].symbol = item.corners[2];
            c[pt].brush.foreground = foreground;
        }
    }

    if (item.corners[3] != U'\0') {
        auto const pt = Point{end.x - 1, end.y - 1};
        if (in_bounds(pt, c.size)) {
            c[pt].symbol = item.corners[3];
            c[pt].brush.foreground = foreground;
        }
    }
}

void put(Canvas c, Point at, shape::Box const& item, Brush const& brush)
{
    // One Past Bottom Right Corner
    auto const end = Point{
        .x = std::min(at.x + item.size.width, c.size.width),
        .y = std::min(at.y + item.size.height, c.size.height),
    };

    // Horizontal
    put(c, {at.x + 1, at.y},
        shape::HLine{
            .length = item.size.width - 2,
            .symbol = item.walls[0],
        },
        brush);
    put(c, {at.x + 1, end.y - 1},
        shape::HLine{
            .length = item.size.width - 2,
            .symbol = item.walls[1],
        },
        brush);

    // Vertical
    put(c, {at.x, at.y + 1},
        shape::VLine{
            .length = item.size.height - 2,
            .symbol = item.walls[3],
        },
        brush);
    put(c, {end.x - 1, at.y + 1},
        shape::VLine{
            .length = item.size.height - 2,
            .symbol = item.walls[2],
        },
        brush);

    auto const in_bounds = [](Point at, Area area) {
        return at.x >= 0 && at.y >= 0 && at.x < area.width && at.y < area.height;
    };

    // Corners
    if (item.corners[0] != U'\0' && in_bounds(at, c.size)) {
        c[at].symbol = item.corners[0];
        c[at].brush = brush;
    }

    if (item.corners[1] != U'\0') {
        auto const pt = Point{end.x - 1, at.y};
        if (in_bounds(pt, c.size)) {
            c[pt].symbol = item.corners[1];
            c[pt].brush = brush;
        }
    }

    if (item.corners[2] != U'\0') {
        auto const pt = Point{at.x, end.y - 1};
        if (in_bounds(pt, c.size)) {
            c[pt].symbol = item.corners[2];
            c[pt].brush = brush;
        }
    }

    if (item.corners[3] != U'\0') {
        auto const pt = Point{end.x - 1, end.y - 1};
        if (in_bounds(pt, c.size)) {
            c[pt].symbol = item.corners[3];
            c[pt].brush = brush;
        }
    }
}

void put(Canvas c, shape::Frame const& item)
{
    put(c, {.x = 0, .y = 0},
        shape::Box{
            .corners = item.corners,
            .walls = item.walls,
            .size = c.size,
        });
}

void put(Canvas c, shape::Frame const& item, Color foreground)
{
    put(c, {.x = 0, .y = 0},
        shape::Box{
            .corners = item.corners,
            .walls = item.walls,
            .size = c.size,
        },
        foreground);
}

void put(Canvas c, shape::Frame const& item, Brush const& brush)
{
    put(c, {.x = 0, .y = 0},
        shape::Box{
            .corners = item.corners,
            .walls = item.walls,
            .size = c.size,
        },
        brush);
}

void fill(Canvas c, Glyph g)
{
    for (auto y = 0; y < c.size.height; ++y) {
        for (auto x = 0; x < c.size.width; ++x) {
            c[{x, y}] = g;
        }
    }
}

void fill(Canvas c, Brush b)
{
    for (auto y = 0; y < c.size.height; ++y) {
        for (auto x = 0; x < c.size.width; ++x) {
            c[{x, y}].brush = b;
        }
    }
}

void fill(Canvas c, char32_t ch)
{
    for (auto y = 0; y < c.size.height; ++y) {
        for (auto x = 0; x < c.size.width; ++x) {
            c[{x, y}].symbol = ch;
        }
    }
}

void fill(Canvas c, ColorBG bg)
{
    for (auto y = 0; y < c.size.height; ++y) {
        for (auto x = 0; x < c.size.width; ++x) {
            c[{x, y}].brush.background = bg.value;
        }
    }
}

void fill(Canvas c, ColorFG fg)
{
    for (auto y = 0; y < c.size.height; ++y) {
        for (auto x = 0; x < c.size.width; ++x) {
            c[{x, y}].brush.foreground = fg.value;
        }
    }
}

void fill(Canvas c, Traits ts)
{
    for (auto y = 0; y < c.size.height; ++y) {
        for (auto x = 0; x < c.size.width; ++x) {
            c[{x, y}].brush.traits = ts;
        }
    }
}

void clear(Canvas c) { fill(c, Glyph{U' '}); }

}  // namespace ox