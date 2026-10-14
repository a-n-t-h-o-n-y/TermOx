#include <ox/radiogroup.hpp>

#include <algorithm>
#include <stdexcept>
#include <utility>

#include <ox/controls.hpp>
#include <ox/label.hpp>
#include <ox/put.hpp>
#include <ox/text_view.hpp>

namespace ox {
RadioGroup::Options const RadioGroup::init = {};
RadioGroup::RadioGroup(Options x)
    : Widget{x.focus_policy, x.width_policy, x.height_policy},

      style{x.style},
      focused_style{x.focused_style},
      on{std::move(x.on)},
      off{std::move(x.off)},
      items_{std::move(x.items)},
      selected_{x.selected_index}
{
    if (selected_ && *selected_ >= items_.size()) {
        throw std::out_of_range{"Radio selection"};
    }
}

RadioGroup::RadioGroup(RadioGroup&& other)
    : Widget{std::move(other)},
      style{std::move(other.style)},
      focused_style{std::move(other.focused_style)},
      on{std::move(other.on)},
      off{std::move(other.off)},
      on_select{std::move(other.on_select)},
      items_{std::move(other.items_)},
      selected_{std::exchange(other.selected_, std::nullopt)},
      highlighted_{std::exchange(other.highlighted_, std::nullopt)},
      measure_layouts_{std::move(other.measure_layouts_)}
{}
auto RadioGroup::operator=(RadioGroup&& other) -> RadioGroup&
{
    if (this == &other) { return *this; }
    Widget::operator=(std::move(other));
    style = std::move(other.style);
    focused_style = std::move(other.focused_style);
    on = std::move(other.on);
    off = std::move(other.off);
    on_select = std::move(other.on_select);
    items_ = std::move(other.items_);
    selected_ = std::exchange(other.selected_, std::nullopt);
    highlighted_ = std::exchange(other.highlighted_, std::nullopt);
    measure_layouts_ = std::move(other.measure_layouts_);
    return *this;
}
auto RadioGroup::select(std::optional<std::size_t> index) -> bool
{
    if (index && *index >= items_.size()) {
        throw std::out_of_range{"Radio selection"};
    }
    auto const changed = selected_ != index;
    if (index) { highlighted_ = index; }
    selected_ = index;
    if (!changed) { return false; }
    request_repaint();
    on_select(index);
    return true;
}
void RadioGroup::set_items(std::vector<std::string> replacement)
{
    for (auto const& text : replacement) {
        if (measure_text(text, {std::numeric_limits<int>::max(), 1}).height != 1) {
            throw std::invalid_argument{"Radio labels must be single-line"};
        }
    }
    auto const changed = selected_.has_value();
    request_layout();
    items_.swap(replacement);
    selected_.reset();
    highlighted_.reset();
    request_repaint();
    if (changed) { on_select(std::nullopt); }
}
auto RadioGroup::measure(Area available, esc::WidthPolicy policy) const -> Area
{
    validate(available);
    auto width = std::size_t{0};
    auto const on_width = esc::cell_width(std::span<Glyph const>{on}, policy);
    auto const off_width = esc::cell_width(std::span<Glyph const>{off}, policy);
    if (!on_width || !off_width) {
        throw std::invalid_argument{"Invalid radio indicator"};
    }
    measure_layouts_.resize(items_.size());
    for (std::size_t i = 0; i < items_.size(); ++i) {
        auto& layout = measure_layouts_[i];
        layout.update(items_[i], available.width, TextWrap::None, policy);
        width = std::max(width, std::max(*on_width, *off_width) + 1 +
                                    (std::size_t)text_content_size(layout).width);
    }
    if (width > (std::size_t)std::numeric_limits<int>::max() ||
        items_.size() > (std::size_t)std::numeric_limits<int>::max()) {
        throw std::length_error{"Radio extent overflow"};
    }
    return {(int)width, (int)items_.size()};
}

auto RadioGroup::paint(Canvas canvas) -> Cursor
{
    auto const clip = canvas.visible();
    for (auto y = clip.at.y;
         (std::size_t)y < items_.size() && y < clip.at.y + clip.size.height; ++y) {
        auto const& glyphs = selected_ == (std::size_t)y ? on : off;
        auto patch = is_focused() && highlighted_ == (std::size_t)y ? focused_style
                                                                    : StylePatch{};
        if (!is_enabled()) { patch = patch | disabled_style; }
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
                    items_[(std::size_t)y], Align::Left, appearance);
    }
    return {};
}

auto RadioGroup::handle(KeyEvent const& event) -> EventResponse
{
    if (items_.empty() || event.action == KeyAction::Release) { return {}; }
    if (activates(event)) {
        auto const normalized = move_selection(highlighted_, items_.size(), 0);
        auto const changed = select(*highlighted_);
        return {.handled = true, .repaint = normalized.repaint || changed};
    }
    if (event.matches(KeyCode::ArrowUp, {}, event.action)) {
        auto const response = move_selection(highlighted_, items_.size(), -1);
        auto const changed = select(highlighted_);
        return {.handled = true, .repaint = response.repaint || changed};
    }
    if (event.matches(KeyCode::ArrowDown, {}, event.action)) {
        auto const response = move_selection(highlighted_, items_.size(), 1);
        auto const changed = select(highlighted_);
        return {.handled = true, .repaint = response.repaint || changed};
    }
    return {};
}
auto RadioGroup::handle(MousePress const& event) -> EventResponse
{
    if (!activates(event) || event.at.y < 0 ||
        (std::size_t)event.at.y >= items_.size()) {
        return {};
    }
    auto const changed = select((std::size_t)event.at.y);
    return {.handled = true, .repaint = changed};
}
auto RadioGroup::focus_in() -> EventResponse
{
    if (selected_ && *selected_ < items_.size()) { highlighted_ = selected_; }
    else {
        (void)move_selection(highlighted_, items_.size(), 0);
    }
    return {.handled = true, .repaint = true};
}
auto RadioGroup::focus_out() -> EventResponse
{ return {.handled = true, .repaint = true}; }
}  // namespace ox
