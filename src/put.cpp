#include <ox/put.hpp>

#include <string_view>

#include <ox/core/core.hpp>

namespace ox {

void put(Canvas& c, Point at, Glyph const& item)
{
    if (at.x < c.size.width && at.y < c.size.height) { c[at] = item; }
}

void put(Canvas& c, Point at, std::string_view item)
{
    put(c, at, esc::detail::utf8_to_glyphs(item));
}

void put(Canvas& c, Point at, std::u32string_view item)
{
    put(c, at, esc::detail::utf32_to_glyphs(item));
}

void put(Canvas& c, Point at, shape::HLine const& item)
{
    if (at.y >= c.size.height || item.symbol == U'\0') { return; }

    auto const end = std::min(at.x + item.length, c.size.width);
    while (at.x < end) {
        c[at].symbol = item.symbol;
        c[at].brush.foreground = item.foreground;
        ++at.x;
    }
}

void put(Canvas& c, Point at, shape::VLine const& item)
{
    if (at.x >= c.size.width || item.symbol == U'\0') { return; }

    auto const end = std::min(at.y + item.length, c.size.height);
    while (at.y < end) {
        c[at].symbol = item.symbol;
        c[at].brush.foreground = item.foreground;
        ++at.y;
    }
}

void put(Canvas& c, Point at, shape::Box const& item)
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
            .foreground = item.foreground,
        });
    put(c, {at.x + 1, end.y - 1},
        shape::HLine{
            .length = item.size.width - 2,
            .symbol = item.walls[1],
            .foreground = item.foreground,
        });

    // Vertical
    put(c, {at.x, at.y + 1},
        shape::VLine{
            .length = item.size.height - 2,
            .symbol = item.walls[3],
            .foreground = item.foreground,
        });
    put(c, {end.x - 1, at.y + 1},
        shape::VLine{
            .length = item.size.height - 2,
            .symbol = item.walls[2],
            .foreground = item.foreground,
        });

    // Corners
    if (item.corners[0] != U'\0') {
        c[at].symbol = item.corners[0];
        c[at].brush.foreground = item.foreground;
    }

    if (item.corners[1] != U'\0') {
        c[{end.x - 1, at.y}].symbol = item.corners[1];
        c[{end.x - 1, at.y}].brush.foreground = item.foreground;
    }

    if (item.corners[2] != U'\0') {
        c[{at.x, end.y - 1}].symbol = item.corners[2];
        c[{at.x, end.y - 1}].brush.foreground = item.foreground;
    }

    if (item.corners[3] != U'\0') {
        c[{end.x - 1, end.y - 1}].symbol = item.corners[3];
        c[{end.x - 1, end.y - 1}].brush.foreground = item.foreground;
    }
}

void put(Canvas& c, Point at, shape::Fill const& item)
{
    auto const end = Point{
        .x = std::min(at.x + item.size.width, c.size.width),
        .y = std::min(at.y + item.size.height, c.size.height),
    };
    for (auto y = at.y; y < end.y; ++y) {
        for (auto x = at.x; x < end.x; ++x) {
            c[{x, y}] = item.glyph;
        }
    }
}

void put(Canvas& c, shape::Corners const& item)
{
    put(c, {.x = 0, .y = 0},
        shape::Box{
            .corners = {U'┌', U'┐', U'└', U'┘'},
            .walls = {U'\0', U'\0', U'\0', U'\0'},
            .foreground = item.foreground,
            .size = c.size,
        });
}

void put(Canvas& c, shape::CornersRound const& item)
{
    put(c, {.x = 0, .y = 0},
        shape::Box{
            .corners = {U'╭', U'╮', U'╰', U'╯'},
            .walls = {U'\0', U'\0', U'\0', U'\0'},
            .foreground = item.foreground,
            .size = c.size,
        });
}

void put(Canvas& c, shape::Frame const& item)
{
    put(c, {.x = 0, .y = 0},
        shape::Box{
            .foreground = item.foreground,
            .size = c.size,
        });
}

void put(Canvas& c, shape::FrameRound const& item)
{
    put(c, {.x = 0, .y = 0},
        shape::Box{
            .corners = {U'╭', U'╮', U'╰', U'╯'},
            .foreground = item.foreground,
            .size = c.size,
        });
}

void put(Canvas& c, shape::FrameBold const& item)
{
    put(c, {.x = 0, .y = 0},
        shape::Box{
            .corners = {U'┏', U'┓', U'┗', U'┛'},
            .walls = {U'━', U'━', U'┃', U'┃'},
            .foreground = item.foreground,
            .size = c.size,
        });
}

void put(Canvas& c, shape::FrameTwin const& item)
{
    put(c, {.x = 0, .y = 0},
        shape::Box{
            .corners = {U'╔', U'╗', U'╚', U'╝'},
            .walls = {U'═', U'═', U'║', U'║'},
            .foreground = item.foreground,
            .size = c.size,
        });
}

void put(Canvas& c, shape::HalfFrame const& item)
{
    put(c, {.x = 0, .y = 0},
        shape::Box{
            .corners = {U'\0', U'\0', U'\0', U'┘'},
            .walls = {U'\0', U'─', U'│', U'\0'},
            .foreground = item.foreground,
            .size = c.size,
        });
}

void put(Canvas& c, shape::HalfFrameRound const& item)
{
    put(c, {.x = 0, .y = 0},
        shape::Box{
            .corners = {U'\0', U'\0', U'\0', U'╯'},
            .walls = {U'\0', U'─', U'│', U'\0'},
            .foreground = item.foreground,
            .size = c.size,
        });
}

void put(Canvas& c, shape::HalfFrameBold const& item)
{
    put(c, {.x = 0, .y = 0},
        shape::Box{
            .corners = {U'\0', U'\0', U'\0', U'┛'},
            .walls = {U'\0', U'━', U'┃', U'\0'},
            .foreground = item.foreground,
            .size = c.size,
        });
}

void put(Canvas& c, shape::HalfFrameTwin const& item)
{
    put(c, {.x = 0, .y = 0},
        shape::Box{
            .corners = {U'\0', U'\0', U'\0', U'╝'},
            .walls = {U'\0', U'═', U'║', U'\0'},
            .foreground = item.foreground,
            .size = c.size,
        });
}

void put(Canvas& c, shape::Brackets const& item)
{
    put(c, {.x = 0, .y = 0},
        shape::Box{
            .corners = {U'┌', U'┐', U'└', U'┘'},
            .walls = {U'\0', U'\0', U'│', U'│'},
            .foreground = item.foreground,
            .size = c.size,
        });
}

void put(Canvas& c, shape::BracketsRound const& item)
{
    put(c, {.x = 0, .y = 0},
        shape::Box{
            .corners = {U'╭', U'╮', U'╰', U'╯'},
            .walls = {U'\0', U'\0', U'│', U'│'},
            .foreground = item.foreground,
            .size = c.size,
        });
}

void put(Canvas& c, shape::BracketsBold const& item)
{
    put(c, {.x = 0, .y = 0},
        shape::Box{
            .corners = {U'┏', U'┓', U'┗', U'┛'},
            .walls = {U'\0', U'\0', U'┃', U'┃'},
            .foreground = item.foreground,
            .size = c.size,
        });
}

void put(Canvas& c, shape::BracketsTwin const& item)
{
    put(c, {.x = 0, .y = 0},
        shape::Box{
            .corners = {U'╔', U'╗', U'╚', U'╝'},
            .walls = {U'\0', U'\0', U'║', U'║'},
            .foreground = item.foreground,
            .size = c.size,
        });
}

void clear(Canvas& c)
{
    put(c, {0, 0}, shape::Fill{.glyph = Glyph{U' '}, .size = c.size});
}

}  // namespace ox