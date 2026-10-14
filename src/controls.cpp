#include <ox/controls.hpp>

#include <algorithm>
#include <cstdint>
#include <utility>

namespace ox {

auto activates(KeyEvent const& event) -> bool
{
    return event.action != KeyAction::Release &&
           (event.matches(KeyCode::Enter, {}, event.action) ||
            event.matches(U' ', {}, event.action));
}
auto activates(MousePress const& event) -> bool
{ return event.button == MouseButton::Left; }
auto releases(KeyEvent const& event) -> bool
{
    return event.action == KeyAction::Release &&
           (event.matches(KeyCode::Enter, event.modifiers, event.action) ||
            event.matches(U' ', event.modifiers, event.action));
}
auto releases(MouseRelease const& event) -> bool
{ return !event.button || event.button == MouseButton::Left; }

auto select_index(std::optional<std::size_t>& selected,
                  std::size_t index,
                  std::size_t count) -> EventResponse
{
    if (index >= count) { return {}; }
    auto const before = std::exchange(selected, index);
    return {.handled = true, .repaint = before != selected};
}

auto move_selection(std::optional<std::size_t>& selected, std::size_t count, int steps)
    -> EventResponse
{
    auto const before = selected;
    if (count == 0) { selected.reset(); }
    else if (!selected) {
        selected = 0;
    }
    else {
        auto const index = std::min(*selected, count - 1);
        selected = steps >= 0
                       ? index + std::min((std::size_t)steps, count - 1 - index)
                       : index - std::min(index, (std::size_t)-(std::int64_t)steps);
    }
    return {.handled = count != 0, .repaint = before != selected};
}
}  // namespace ox
