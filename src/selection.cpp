#include <algorithm>
#include <limits>
#include <ox/put.hpp>
#include <ox/selection.hpp>
#include <ox/text_view.hpp>
#include <stdexcept>
namespace ox {
void validate_row_text(std::string_view text)
{
    for (auto c : esc::utf8_to_utf32(text)) {
        if (c < U' ' || (c >= U'\x7f' && c <= U'\x9f')) {
            throw std::invalid_argument{"Rows require single-line printable text"};
        }
    }
}
auto select_rows(RowSelection& state,
                 std::size_t row,
                 std::size_t count,
                 SelectionAction action,
                 std::function<bool(std::size_t)> const& enabled) -> bool
{
    if (row >= count) { throw std::out_of_range{"Selection row"}; }
    if (enabled && !enabled(row)) { return false; }
    auto next = state;
    next.current = row;
    switch (action) {
        case SelectionAction::Replace:
            next.rows = {row};
            next.anchor = row;
            break;
        case SelectionAction::Extend: {
            auto const anchor = state.anchor.value_or(state.current.value_or(row));
            if (anchor >= count) { throw std::out_of_range{"Selection anchor"}; }
            next.anchor = anchor;
            next.rows.clear();
            for (auto i = std::min(anchor, row); i <= std::max(anchor, row); ++i) {
                if (!enabled || enabled(i)) { next.rows.push_back(i); }
            }
            break;
        }
        case SelectionAction::Toggle: {
            auto it = std::ranges::lower_bound(next.rows, row);
            if (it != next.rows.end() && *it == row) { next.rows.erase(it); }
            else {
                next.rows.insert(it, row);
            }
            next.anchor = row;
            break;
        }
        case SelectionAction::Move: break;
        default: throw std::invalid_argument{"Selection action"};
    }
    if (next == state) { return false; }
    state = std::move(next);
    return true;
}
auto row_navigation(KeyEvent const& event, int page) -> std::optional<int>
{
    if (event.action == KeyAction::Release || event.modifiers.alt ||
        event.modifiers.meta || event.modifiers.super || event.modifiers.hyper) {
        return {};
    }
    auto matches = [&](KeyCode k) {
        return event.matches(k, event.modifiers, event.action);
    };
    if (matches(KeyCode::ArrowUp)) { return -1; }
    if (matches(KeyCode::ArrowDown)) { return 1; }
    if (matches(KeyCode::PageUp)) { return -std::max(1, page); }
    if (matches(KeyCode::PageDown)) { return std::max(1, page); }
    if (matches(KeyCode::Home)) { return -std::numeric_limits<int>::max(); }
    if (matches(KeyCode::End)) { return std::numeric_limits<int>::max(); }
    return {};
}
auto row_target(std::optional<std::size_t> current,
                std::size_t count,
                int steps,
                std::function<bool(std::size_t)> const& enabled)
    -> std::optional<std::size_t>
{
    if (!count) { return {}; }
    if (count > (std::size_t)std::numeric_limits<int>::max()) {
        throw std::length_error{"Row count"};
    }
    auto at = steps == std::numeric_limits<int>::max()    ? (std::int64_t)count - 1
              : steps == -std::numeric_limits<int>::max() ? 0
              : current ? std::clamp((std::int64_t)*current + steps, std::int64_t{0},
                                     (std::int64_t)count - 1)
                        : 0;
    auto const direction = steps < 0 ? -1 : 1;
    for (auto i = at; i >= 0 && i < (std::int64_t)count; i += direction) {
        if (!enabled || enabled((std::size_t)i)) { return (std::size_t)i; }
    }
    // A disabled endpoint must not strand an otherwise valid cursor.
    for (auto i = at; i >= 0 && i < (std::int64_t)count; i -= direction) {
        if (!enabled || enabled((std::size_t)i)) { return (std::size_t)i; }
    }
    return {};
}
auto visible_rows(Canvas canvas, int offset, std::size_t count) -> std::pair<int, int>
{
    if (offset < 0 || count > (std::size_t)std::numeric_limits<int>::max()) {
        throw std::invalid_argument{"Row extent"};
    }
    auto const clip = canvas.visible();
    auto const first = std::clamp((std::int64_t)offset + clip.at.y, std::int64_t{0},
                                  (std::int64_t)count);
    auto const last = std::clamp(first + clip.size.height, first, (std::int64_t)count);
    return {(int)first, (int)last};
}
void paint_row_text(Canvas canvas,
                    std::string_view text,
                    Align align,
                    Style style,
                    TextLayout& layout)
{
    fill(canvas, style);
    auto const width = canvas.size().width;
    if (!width || !canvas.visible().size.width || !canvas.visible().size.height) {
        return;
    }
    layout.update(text, width, TextWrap::None, canvas.width_policy());
    auto const columns = layout.lines().front().columns;
    if (columns <= width) {
        put_text_line(canvas, {align_offset(align, width, columns), 0}, layout, 0,
                      style, style);
        return;
    }
    auto const marker = esc::grapheme_width(U"…", canvas.width_policy());
    auto const remaining = marker && *marker > 0 && *marker <= (std::size_t)width
                               ? width - (int)*marker
                               : width;
    put_text_line(canvas.subcanvas({}, {remaining, 1}), {}, layout, 0, style, style);
    if (remaining != width) { put(canvas, {remaining, 0}, U"…", style); }
}
}  // namespace ox
