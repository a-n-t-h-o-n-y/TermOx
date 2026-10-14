#include <ox/text_view.hpp>

#include <algorithm>
#include <cstdint>
#include <limits>

#include <ox/put.hpp>
#include <ox/scroll.hpp>

namespace ox {
namespace {
auto row_origin(TextLayout const& layout, int row, Area size, Align align, Point scroll)
    -> Point
{
    auto const columns = layout.lines()[(std::size_t)row].columns;
    auto const x = columns < size.width ? align_offset(align, size.width, columns) -
                                              std::max(scroll.x, 0)
                                        : -std::max(scroll.x, 0);
    return {x, row - scroll.y};
}

void reveal(TextEdit const& editor,
            TextLayout& layout,
            Point& scroll,
            Area size,
            TextWrap wrap,
            Align align)
{
    layout.update(editor.text(), size.width, wrap, layout.policy(), layout.tab_width());
    scroll = text_scroll(layout, editor.caret(), size, align, scroll);
}
}  // namespace

auto text_scroll(TextLayout const& layout,
                 std::size_t caret,
                 Area size,
                 Align align,
                 Point scroll) -> Point
{
    validate(size);
    auto const point = layout.position(caret);
    auto const columns = layout.lines()[(std::size_t)point.y].columns;
    scroll.y = clamp_scroll(scroll.y, (int)layout.lines().size(), size.height);
    scroll.y =
        scroll_to_show(scroll.y, point.y, 1, (int)layout.lines().size(), size.height);
    if (size.width == 0) { scroll.x = 0; }
    else if (columns < size.width) {
        // Leave a cell for the caret when right alignment reaches the final column.
        scroll.x =
            point.x + align_offset(align, size.width, columns) >= size.width ? 1 : 0;
    }
    else {
        scroll.x = std::clamp(scroll.x, 0, columns);
        // Direct scroll changes must also land on a complete grapheme.
        auto const offset = layout.offset({scroll.x, point.y});
        scroll.x = scroll.x == columns ? columns : layout.position(offset).x;
        if (point.x < scroll.x) { scroll.x = point.x; }
        if (point.x - scroll.x >= size.width) {
            auto const desired = point.x - size.width + 1;
            auto boundary = layout.offset({desired, point.y});
            if (layout.position(boundary).x < desired) {
                boundary = *std::ranges::upper_bound(layout.boundaries(), boundary);
            }
            scroll.x = layout.position(boundary).x;
        }
    }
    return scroll;
}

auto text_position(TextLayout const& layout,
                   std::size_t offset,
                   Area size,
                   Align align,
                   Point scroll) -> Point
{
    validate(size);
    scroll.y = clamp_scroll(scroll.y, (int)layout.lines().size(), size.height);
    auto const point = layout.position(offset);
    auto const origin = row_origin(layout, point.y, size, align, scroll);
    return {point.x + origin.x, origin.y};
}

auto text_offset(TextLayout const& layout,
                 Point at,
                 Area size,
                 Align align,
                 Point scroll) -> std::size_t
{
    validate(size);
    scroll.y = clamp_scroll(scroll.y, (int)layout.lines().size(), size.height);
    auto const row = (int)std::clamp(std::int64_t{scroll.y} + at.y, std::int64_t{0},
                                     (std::int64_t)layout.lines().size() - 1);
    auto const origin = row_origin(layout, row, size, align, scroll);
    auto const column = (int)std::clamp(std::int64_t{at.x} - origin.x, std::int64_t{0},
                                        (std::int64_t)std::numeric_limits<int>::max());
    return layout.offset({column, row});
}

void paint_text(Canvas canvas,
                TextLayout const& layout,
                Align align,
                Point scroll,
                Style normal,
                Style selected,
                std::pair<std::size_t, std::size_t> selection)
{
    scroll.y = clamp_scroll(scroll.y, (int)layout.lines().size(), canvas.size().height);
    auto const clip = canvas.visible();
    auto const end = std::min((std::int64_t)layout.lines().size(),
                              std::int64_t{scroll.y} + clip.at.y + clip.size.height);
    for (auto row = std::int64_t{scroll.y} + clip.at.y; row < end; ++row) {
        put_text_line(canvas,
                      row_origin(layout, (int)row, canvas.size(), align, scroll),
                      layout, (std::size_t)row, normal, selected, selection);
    }
}

auto edit_text(TextEdit& editor,
               TextLayout& layout,
               Point& scroll,
               Area size,
               TextWrap wrap,
               Align align,
               KeyEvent const& key,
               std::function<bool(char32_t)> const& accept) -> EventResponse
{
    validate(size);
    if (key.action == KeyAction::Release) { return {}; }
    layout.update(editor.text(), size.width, wrap, layout.policy(), layout.tab_width());
    auto const response = edit_key(editor, key, layout, accept);
    if (response.repaint) { reveal(editor, layout, scroll, size, wrap, align); }
    return response;
}

auto edit_text(TextEdit& editor,
               TextLayout& layout,
               Point& scroll,
               Area size,
               TextWrap wrap,
               Align align,
               esc::PasteChunk const& chunk,
               std::function<bool(char32_t)> const& accept) -> EventResponse
{
    validate(size);
    auto const changed = editor.paste(chunk, accept);
    if (changed) { reveal(editor, layout, scroll, size, wrap, align); }
    return {.handled = true, .repaint = changed};
}
}  // namespace ox
