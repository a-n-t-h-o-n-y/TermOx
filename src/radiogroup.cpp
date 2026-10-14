#include <ox/radiogroup.hpp>

#include <algorithm>
#include <stdexcept>
#include <utility>

#include <ox/controls.hpp>
#include <ox/label.hpp>
#include <ox/put.hpp>
#include <ox/scroll.hpp>
#include <ox/selection.hpp>
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
    for (auto const& text : items_) {
        validate_row_text(text);
    }
    enabled_items_.assign(items_.size(), true);
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
      on_scroll{std::move(other.on_scroll)},
      items_{std::move(other.items_)},
      selected_{std::exchange(other.selected_, std::nullopt)},
      highlighted_{std::exchange(other.highlighted_, std::nullopt)},
      measure_layouts_{std::move(other.measure_layouts_)},
      enabled_items_{std::move(other.enabled_items_)},
      scroll_{std::exchange(other.scroll_, 0)}
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
    on_scroll = std::move(other.on_scroll);
    notified_scroll_.reset();
    items_ = std::move(other.items_);
    selected_ = std::exchange(other.selected_, std::nullopt);
    highlighted_ = std::exchange(other.highlighted_, std::nullopt);
    measure_layouts_ = std::move(other.measure_layouts_);
    enabled_items_ = std::move(other.enabled_items_);
    scroll_ = std::exchange(other.scroll_, 0);
    return *this;
}
auto RadioGroup::select(std::optional<std::size_t> index) -> bool
{
    if (index && *index >= items_.size()) {
        throw std::out_of_range{"Radio selection"};
    }
    if (index && !enabled_items_[*index]) { return false; }
    auto const changed = selected_ != index;
    auto const old_scroll = scroll_;
    if (index) {
        scroll_ =
            scroll_to_show(scroll_, (int)*index, 1, (int)items_.size(), size().height);
    }
    if (index) { highlighted_ = index; }
    selected_ = index;
    auto lifetime = track(*this);
    auto scrolled = old_scroll != scroll_;
    if (changed || scrolled) request_repaint();
    notify_scroll();
    if (auto* self = lifetime.try_get(); self && changed) self->on_select(index);
    return changed || scrolled;
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
    auto enabled = std::vector<bool>(replacement.size(), true);
    items_.swap(replacement);
    enabled_items_.swap(enabled);
    scroll_ = 0;
    selected_.reset();
    highlighted_.reset();
    request_repaint();
    auto lifetime = track(*this);
    notify_scroll();
    if (auto* self = lifetime.try_get(); self && changed) self->on_select(std::nullopt);
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
    fill(canvas, appearance(style));
    auto const [first, last] = visible_rows(canvas, scroll_, items_.size());
    for (auto row = first; row < last; ++row) {
        auto y = row - scroll_;
        auto const& glyphs = selected_ == (std::size_t)row ? on : off;
        auto patch = is_focused() && highlighted_ == (std::size_t)row ? focused_style
                                                                      : StylePatch{};
        if (!is_enabled() || !enabled_items_[(std::size_t)row]) {
            patch = patch | disabled_style;
        }
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
                    items_[(std::size_t)row], Align::Left, appearance);
    }
    return {};
}

auto RadioGroup::handle(KeyEvent const& event) -> EventResponse
{
    if (!is_enabled() || items_.empty() || event.action == KeyAction::Release) {
        return {};
    }
    auto steps = row_navigation(event, size().height);
    if (!steps && !activates(event)) { return {}; }
    auto target =
        row_target(highlighted_.has_value() ? highlighted_ : selected_, items_.size(),
                   steps.value_or(0), [&](auto i) { return enabled_items_[i]; });
    if (!target) { return {}; }
    highlighted_ = target;
    auto changed = select(target);
    return {.handled = true, .repaint = changed};
}
auto RadioGroup::handle(MousePress const& event) -> EventResponse
{
    if (!is_enabled() || !activates(event) || !contains({{}, size()}, event.at) ||
        (std::size_t)(event.at.y + scroll_) >= items_.size()) {
        return {};
    }
    auto const changed = select((std::size_t)(event.at.y + scroll_));
    return {.handled = true, .repaint = changed};
}
auto RadioGroup::content_size() const -> Area
{
    if (items_.size() > (std::size_t)std::numeric_limits<int>::max())
        throw std::length_error{"Radio extent overflow"};
    return {size().width, (int)items_.size()};
}
void RadioGroup::notify_scroll()
{
    auto value = std::tuple{scroll_offset(), content_size(), viewport_size()};
    if (notified_scroll_ == value) return;
    notified_scroll_ = value;
    on_scroll(std::get<0>(value), std::get<1>(value), std::get<2>(value));
}
void RadioGroup::on_arrange(Rect)
{
    scroll_ = clamp_scroll(scroll_, (int)items_.size(), size().height);
    notify_scroll();
}
void RadioGroup::refresh()
{
    request_layout();
    request_repaint();
    on_arrange(bounds());
}
auto RadioGroup::set_scroll_offset(Point at) -> bool
{
    auto value = clamp_scroll(at.y, (int)items_.size(), size().height);
    if (value == scroll_) { return false; }
    scroll_ = value;
    request_repaint();
    notify_scroll();
    return true;
}
auto RadioGroup::handle(MouseWheel const& e) -> EventResponse
{
    auto changed =
        set_scroll_offset({0, scroll_by(scroll_, e.dy, (int)items_.size(), size().height)});
    return {.handled = changed, .repaint = changed};
}
void RadioGroup::set_item_enabled(std::size_t index, bool enabled)
{
    if (index >= items_.size()) { throw std::out_of_range{"Radio item"}; }
    if (enabled_items_[index] == enabled) { return; }
    enabled_items_[index] = enabled;
    request_repaint();
    if (!enabled && highlighted_ == index) {
        highlighted_ =
            row_target({}, items_.size(), 0, [&](auto i) { return enabled_items_[i]; });
    }
    if (!enabled && selected_ == index) { select(std::nullopt); }
}
auto RadioGroup::focus_in() -> EventResponse
{
    if (selected_ && *selected_ < items_.size()) { highlighted_ = selected_; }
    else {
        highlighted_ =
            row_target({}, items_.size(), 0, [&](auto i) { return enabled_items_[i]; });
    }
    return {.handled = true, .repaint = true};
}
auto RadioGroup::focus_out() -> EventResponse
{ return {.handled = true, .repaint = true}; }
}  // namespace ox
