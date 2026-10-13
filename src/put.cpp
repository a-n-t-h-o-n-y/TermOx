#include <ox/put.hpp>

#include <algorithm>
#include <cstdint>
#include <limits>
#include <optional>
#include <stdexcept>

namespace {
using namespace ox;

// Validate the complete text before modifying any cells.
void validate_text(std::u32string_view text, esc::WidthPolicy policy)
{
    for (auto cluster : esc::graphemes(text)) {
        auto width = esc::grapheme_width(cluster, policy);
        if (!width || *width == 0) {
            throw std::invalid_argument{"Expected printable text"};
        }
    }
}

auto appearance(Canvas c,
                Point at,
                std::optional<Style> style,
                std::optional<Color> color) -> Style
{
    auto result = style.value_or(c[at].style);
    if (color) { result.foreground = *color; }
    return result;
}

void validate_symbol(Canvas c, char32_t ch)
{
    if (ch != U'\0' &&
        esc::grapheme_width(std::u32string_view{&ch, 1}, c.width_policy()) != 1) {
        throw std::invalid_argument{"Shape symbols must occupy one column"};
    }
}

void symbol(Canvas c,
            std::int64_t x,
            std::int64_t y,
            char32_t ch,
            std::optional<Style> style,
            std::optional<Color> color)
{
    if (ch == U'\0') { return; }
    auto const clip = c.visible();
    if (x < clip.at.x || y < clip.at.y ||
        x >= (std::int64_t)clip.at.x + clip.size.width ||
        y >= (std::int64_t)clip.at.y + clip.size.height) {
        return;
    }
    auto const text = std::u32string_view{&ch, 1};
    auto const point = Point{(int)x, (int)y};
    c.write(point, text, appearance(c, point, style, color));
}

void line(Canvas c,
          std::int64_t x,
          std::int64_t y,
          char32_t ch,
          int length,
          bool horizontal,
          std::optional<Style> style,
          std::optional<Color> color)
{
    if (length < 0) { throw std::invalid_argument{"Negative line length"}; }
    if (length == 0 || ch == U'\0') { return; }
    validate_symbol(c, ch);
    auto const clip = c.visible();
    auto const begin = std::max(horizontal ? x : y,
                                (std::int64_t)(horizontal ? clip.at.x : clip.at.y));
    auto const end = std::min((horizontal ? x : y) + length,
                              (std::int64_t)(horizontal ? clip.at.x : clip.at.y) +
                                  (horizontal ? clip.size.width : clip.size.height));
    for (auto i = begin; i < end; ++i) {
        symbol(c, horizontal ? i : x, horizontal ? y : i, ch, style, color);
    }
}

void box(Canvas c,
         Point at,
         shape::Box const& item,
         Area size,
         std::optional<Style> style,
         std::optional<Color> color)
{
    validate(size);
    for (auto ch : item.walls) {
        validate_symbol(c, ch);
    }
    for (auto ch : item.corners) {
        validate_symbol(c, ch);
    }
    if (size.width == 0 || size.height == 0) { return; }
    auto const right = (std::int64_t)at.x + size.width - 1;
    auto const bottom = (std::int64_t)at.y + size.height - 1;
    line(c, (std::int64_t)at.x + 1, at.y, item.walls[0], std::max(size.width - 2, 0),
         true, style, color);
    if (size.height > 1) {
        line(c, (std::int64_t)at.x + 1, bottom, item.walls[1],
             std::max(size.width - 2, 0), true, style, color);
    }
    line(c, at.x, (std::int64_t)at.y + 1, item.walls[3], std::max(size.height - 2, 0),
         false, style, color);
    if (size.width > 1) {
        line(c, right, (std::int64_t)at.y + 1, item.walls[2],
             std::max(size.height - 2, 0), false, style, color);
    }
    symbol(c, at.x, at.y, item.corners[0], style, color);
    if (size.width > 1) { symbol(c, right, at.y, item.corners[1], style, color); }
    if (size.height > 1) { symbol(c, at.x, bottom, item.corners[2], style, color); }
    if (size.width > 1 && size.height > 1) {
        symbol(c, right, bottom, item.corners[3], style, color);
    }
}

void restyle(Canvas c, auto modify)
{
    auto const clip = c.visible();
    for (int y = clip.at.y; y < clip.at.y + clip.size.height; ++y) {
        for (int x = clip.at.x; x < clip.at.x + clip.size.width;) {
            auto const& cell = c[{x, y}];
            if (cell.columns == 0) {
                ++x;
                continue;
            }
            auto style = cell.style;
            modify(style);
            c.restyle({x, y}, style);
            x += std::min((int)cell.columns, clip.at.x + clip.size.width - x);
        }
    }
}

void fill_symbol(Canvas c, char32_t ch, std::optional<Style> style)
{
    auto const text = std::u32string_view{&ch, 1};
    auto const width = esc::grapheme_width(text, c.width_policy());
    if (!width || *width == 0) {
        throw std::invalid_argument{"Expected a printable fill glyph"};
    }
    auto const clip = c.visible();
    for (int y = clip.at.y; y < clip.at.y + clip.size.height; ++y) {
        // Clear complete old footprints first, leaving partial boundary graphemes
        // untouched.
        for (int x = clip.at.x; x < clip.at.x + clip.size.width; ++x) {
            c.write({x, y}, U" ", style.value_or(c[{x, y}].style));
        }
        auto const start = ((std::size_t)clip.at.x + *width - 1) / *width * *width;
        for (auto x = start; x + *width <= (std::size_t)(clip.at.x + clip.size.width);
             x += *width) {
            c.write({(int)x, y}, text, style.value_or(c[{(int)x, y}].style));
        }
    }
}
}  // namespace

namespace ox {
void put(Canvas c, Point at, Glyph const& item)
{ c.write(at, std::u32string_view{&item.symbol, 1}, item.style); }

void put(Canvas c, Point at, std::string_view text)
{ put(c, at, esc::utf8_to_utf32(text)); }

void put(Canvas c, Point at, std::u32string_view text)
{
    validate_text(text, c.width_policy());
    auto x = (std::int64_t)at.x;
    auto const clip = c.visible();
    for (auto cluster : esc::graphemes(text)) {
        auto const width = *esc::grapheme_width(cluster, c.width_policy());
        if (x >= (std::int64_t)clip.at.x + clip.size.width) { break; }
        if (x >= std::numeric_limits<int>::min()) { c.write({(int)x, at.y}, cluster); }
        x += (std::int64_t)width;
    }
}

void put(Canvas c, Point at, std::span<Glyph const> glyphs)
{
    auto text = std::u32string{};
    text.reserve(glyphs.size());
    for (auto const& glyph : glyphs) {
        if (glyph.style.background.valueless_by_exception() ||
            glyph.style.foreground.valueless_by_exception()) {
            throw std::invalid_argument{"Expected valid glyph styles"};
        }
        text.push_back(glyph.symbol);
    }
    validate_text(text, c.width_policy());
    auto remaining = glyphs;
    for (auto cluster : esc::graphemes(text)) {
        auto const style = remaining.front().style;
        for (auto const& glyph : remaining.first(cluster.size())) {
            if (glyph.style != style) {
                throw std::invalid_argument{"One grapheme requires one style"};
            }
        }
        remaining = remaining.subspan(cluster.size());
    }
    remaining = glyphs;
    auto x = (std::int64_t)at.x;
    auto const clip = c.visible();
    for (auto cluster : esc::graphemes(text)) {
        auto const width = *esc::grapheme_width(cluster, c.width_policy());
        if (x >= (std::int64_t)clip.at.x + clip.size.width) { break; }
        c.write({(int)x, at.y}, cluster, remaining.front().style);
        remaining = remaining.subspan(cluster.size());
        x += (std::int64_t)width;
    }
}

void put(Canvas c, Point at, shape::HLine item, int length)
{ line(c, at.x, at.y, item.symbol, length, true, {}, {}); }
void put(Canvas c, Point at, shape::HLine item, int length, Color foreground)
{ line(c, at.x, at.y, item.symbol, length, true, {}, foreground); }
void put(Canvas c, Point at, shape::HLine item, int length, Style const& style)
{ line(c, at.x, at.y, item.symbol, length, true, style, {}); }
void put(Canvas c, Point at, shape::VLine item, int length)
{ line(c, at.x, at.y, item.symbol, length, false, {}, {}); }
void put(Canvas c, Point at, shape::VLine item, int length, Color foreground)
{ line(c, at.x, at.y, item.symbol, length, false, {}, foreground); }
void put(Canvas c, Point at, shape::VLine item, int length, Style const& style)
{ line(c, at.x, at.y, item.symbol, length, false, style, {}); }

void put(Canvas c, shape::Box const& item) { box(c, {0, 0}, item, c.size(), {}, {}); }
void put(Canvas c, Point at, shape::Box const& item, Area size)
{ box(c, at, item, size, {}, {}); }
void put(Canvas c, shape::Box const& item, Color foreground)
{ box(c, {0, 0}, item, c.size(), {}, foreground); }
void put(Canvas c, Point at, shape::Box const& item, Area size, Color foreground)
{ box(c, at, item, size, {}, foreground); }
void put(Canvas c, shape::Box const& item, Style const& style)
{ box(c, {0, 0}, item, c.size(), style, {}); }
void put(Canvas c, Point at, shape::Box const& item, Area size, Style const& style)
{ box(c, at, item, size, style, {}); }

void fill(Canvas c, Glyph glyph) { fill_symbol(c, glyph.symbol, glyph.style); }
void fill(Canvas c, Style style)
{
    restyle(c, [&](Style& s) { s = style; });
}
void fill(Canvas c, char32_t ch) { fill_symbol(c, ch, {}); }
void fill(Canvas c, ColorBG color)
{
    restyle(c, [&](Style& s) { s.background = color.value; });
}
void fill(Canvas c, ColorFG color)
{
    restyle(c, [&](Style& s) { s.foreground = color.value; });
}
void fill(Canvas c, Traits traits)
{
    restyle(c, [&](Style& s) { s.traits = traits; });
}
void clear(Canvas c) { fill(c, Glyph{}); }
void put_text_line(Canvas canvas,
                   Point at,
                   TextLayout const& layout,
                   std::size_t row,
                   Style normal,
                   Style selected,
                   std::pair<std::size_t, std::size_t> selection)
{
    if (row >= layout.lines().size()) { throw std::out_of_range{"Text row"}; }
    auto const line = layout.lines()[row];
    auto column = 0;
    auto offset = line.begin;
    for (auto cluster :
         esc::graphemes(layout.text().substr(line.begin, line.end - line.begin))) {
        auto const appearance =
            offset < selection.second && offset + cluster.size() > selection.first
                ? selected
                : normal;
        auto const display =
            display_grapheme(cluster, column, layout.policy(), layout.tab_width());
        put(canvas, translate(at, {column, 0}), esc::glyphs(display, appearance));
        column += (int)*esc::cell_width(display, layout.policy());
        offset += cluster.size();
    }
    if (line.next > line.end && selection.first <= line.end &&
        selection.second > line.end) {
        canvas.write(translate(at, {column, 0}), U" ", selected);
    }
}

}  // namespace ox
