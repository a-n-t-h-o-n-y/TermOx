#include <ox/checkbox.hpp>

#include <limits>
#include <utility>

#include <ox/controls.hpp>
#include <ox/put.hpp>
#include <ox/text_view.hpp>

namespace ox {
CheckBox::Options const CheckBox::init = {};
CheckBox::CheckBox(Options x)
    : Widget{x.focus_policy, x.width_policy, x.height_policy},
      label{std::move(x.label)},
      checked_glyphs{std::move(x.checked_glyphs)},
      unchecked_glyphs{std::move(x.unchecked_glyphs)},
      focused_style{x.focused_style},
      checked_{x.checked}
{}
auto CheckBox::set_checked(bool value) -> bool
{
    if (std::exchange(checked_, value) == value) { return false; }
    request_repaint();
    on_change(value);
    return true;
}
auto CheckBox::toggle() -> bool { return set_checked(!checked_); }
auto CheckBox::handle(KeyEvent const& event) -> EventResponse
{
    if (!is_enabled() || !activates(event)) { return {}; }
    auto const changed = toggle();
    return {.handled = true, .repaint = changed};
}
auto CheckBox::handle(MousePress const& event) -> EventResponse
{
    if (!is_enabled() || !activates(event)) { return {}; }
    auto const changed = toggle();
    return {.handled = true, .repaint = changed};
}
auto CheckBox::measure(Area available, esc::WidthPolicy policy) const -> Area
{
    validate(available);
    auto const a = esc::cell_width(std::span<Glyph const>{checked_glyphs}, policy);
    auto const b = esc::cell_width(std::span<Glyph const>{unchecked_glyphs}, policy);
    if (!a || !b) { throw std::invalid_argument{"Invalid checkbox indicator"}; }
    auto label_size = measure_label(measure_layout_, label.text, label.vertical, policy);
    auto const width =
        std::max(*a, *b) +
        (label.text.empty()
             ? 0
             : 1 + (std::size_t)label_size.width);
    if (width > (std::size_t)std::numeric_limits<int>::max()) {
        throw std::length_error{"Checkbox width overflow"};
    }
    return {(int)width, std::max(1, label_size.height)};
}
auto CheckBox::paint(Canvas canvas) -> Cursor
{
    auto const& glyphs = checked_ ? checked_glyphs : unchecked_glyphs;
    auto patch = is_focused() ? focused_style : StylePatch{};
    if (!is_enabled()) { patch = patch | disabled_style; }
    paint_label(canvas, "", Align::Left, appearance(label.style));
    put(canvas, {0, 0}, glyphs, patch);
    auto const width =
        esc::cell_width(std::span<Glyph const>{glyphs}, canvas.width_policy());
    if (!width) { throw std::invalid_argument{"Invalid checkbox indicator"}; }
    if (!label.text.empty() && *width < (std::size_t)canvas.size().width) {
        auto const at = (int)*width + 1;
        paint_label(canvas.subcanvas({at, 0}, {std::max(canvas.size().width - at, 0),
                                               canvas.size().height}),
                    label.text, label.align, label.style | patch, label.vertical);
    }
    return {};
}
}  // namespace ox
