#include <ox/checkbox.hpp>

#include <utility>

#include <ox/controls.hpp>
#include <ox/put.hpp>

namespace ox {
CheckBox::Options const CheckBox::init = {};
CheckBox::CheckBox(Options x)
    : Widget{x.focus_policy, x.size_policy},
      checked{x.checked},
      checked_glyphs{std::move(x.checked_glyphs)},
      unchecked_glyphs{std::move(x.unchecked_glyphs)},
      focused_style{x.focused_style}
{}
auto CheckBox::set_checked(bool value) -> bool
{
    if (std::exchange(checked, value) == value) { return false; }
    request_repaint();
    on_toggle(value);
    return true;
}
auto CheckBox::toggle() -> bool { return set_checked(!checked); }
auto CheckBox::handle(KeyEvent const& event) -> EventResponse
{
    if (!activates(event)) { return {}; }
    auto const changed = toggle();
    return {.handled = true, .repaint = changed};
}
auto CheckBox::handle(MousePress const& event) -> EventResponse
{
    if (!activates(event)) { return {}; }
    auto const changed = toggle();
    return {.handled = true, .repaint = changed};
}
auto CheckBox::paint(Canvas canvas) -> Cursor
{
    auto const& glyphs = checked ? checked_glyphs : unchecked_glyphs;
    put(canvas, {0, 0}, glyphs, is_focused() ? focused_style : StylePatch{});
    return {};
}
}  // namespace ox
