#include <ox/radiogroup.hpp>

#include <algorithm>
#include <stdexcept>
#include <utility>

#include <ox/controls.hpp>
#include <ox/label.hpp>
#include <ox/put.hpp>

namespace ox {
RadioGroup::Options const RadioGroup::init = {};
RadioGroup::RadioGroup(Options x)
    : Widget{x.focus_policy, x.size_policy},
      items{std::move(x.items)},
      selected_index{x.selected_index},
      style{x.style},
      focused_style{x.focused_style},
      on{std::move(x.on)},
      off{std::move(x.off)}
{}

auto RadioGroup::select(std::size_t index) -> bool
{
    auto const response = select_index(selected_index, index, items.size());
    if (!response.handled) { return false; }
    auto const changed = response.repaint || highlighted != index;
    highlighted = index;
    if (changed) { request_repaint(); }
    if (response.repaint) {
        auto const value =
            items[index];  // Slots may replace items or destroy this widget.
        on_select(value);
    }
    return changed;
}

auto RadioGroup::paint(Canvas canvas) -> Cursor
{
    auto const clip = canvas.visible();
    for (auto y = clip.at.y;
         (std::size_t)y < items.size() && y < clip.at.y + clip.size.height; ++y) {
        auto const& glyphs = selected_index == (std::size_t)y ? on : off;
        auto const patch = is_focused() && highlighted == (std::size_t)y ? focused_style
                                                                         : StylePatch{};
        auto const width =
            esc::cell_width(std::span<Glyph const>{glyphs}, canvas.width_policy());
        if (!width) {
            throw std::invalid_argument{"Radio indicator requires known widths"};
        }
        put(canvas, {0, y}, glyphs, patch);
        if (*width >= (std::size_t)canvas.size().width) { continue; }
        auto const x = (int)*width;
        auto const appearance = style | patch;
        canvas.write({x, y}, U" ", appearance);
        paint_label(canvas.subcanvas({x + 1, y}, {canvas.size().width - x - 1, 1}),
                    items[(std::size_t)y], Align::Left, appearance);
    }
    return {};
}

auto RadioGroup::handle(KeyEvent const& event) -> EventResponse
{
    if (items.empty() || event.action == KeyAction::Release) { return {}; }
    if (activates(event)) {
        auto const normalized = move_selection(highlighted, items.size(), 0);
        auto const changed = select(*highlighted);
        return {.handled = true, .repaint = normalized.repaint || changed};
    }
    if (event.matches(KeyCode::ArrowUp, {}, event.action)) {
        return move_selection(highlighted, items.size(), -1);
    }
    if (event.matches(KeyCode::ArrowDown, {}, event.action)) {
        return move_selection(highlighted, items.size(), 1);
    }
    return {};
}
auto RadioGroup::handle(MousePress const& event) -> EventResponse
{
    if (!activates(event) || event.at.y < 0 ||
        (std::size_t)event.at.y >= items.size()) {
        return {};
    }
    auto const changed = select((std::size_t)event.at.y);
    return {.handled = true, .repaint = changed};
}
auto RadioGroup::focus_in() -> EventResponse
{
    if (selected_index && *selected_index < items.size()) {
        highlighted = selected_index;
    }
    else {
        (void)move_selection(highlighted, items.size(), 0);
    }
    return {.handled = true, .repaint = true};
}
auto RadioGroup::focus_out() -> EventResponse
{ return {.handled = true, .repaint = true}; }
}  // namespace ox
