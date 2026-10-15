#include <algorithm>
#include <limits>
#include <ox/controls.hpp>
#include <ox/listview.hpp>
#include <ox/put.hpp>
#include <ox/scroll.hpp>
#include <ox/text_view.hpp>
#include <stdexcept>
namespace ox {
ListView::ListView() : ListView{Options{}} {}
ListView::ListView(Options x)
    : Widget{x.focus_policy, x.width_policy, x.height_policy},
      multiple{x.multiple},
      style{x.style},
      role{x.role},
      selected_style{x.selected_style},
      current_style{x.current_style},
      focused_style{x.focused_style},
      blurred_style{x.blurred_style},
      inactive_selected_style{x.inactive_selected_style},
      alternate_style{x.alternate_style},
      hover_style{x.hover_style},
      indicator_style{x.indicator_style},
      empty_style{x.empty_style},
      striped{x.striped},
      hover_rows{x.hover_rows},
      show_indicator{x.show_indicator},
      empty_text{std::move(x.empty_text)}
{ set_items(std::move(x.items)); }
void ListView::set_items(std::vector<ListItem> items)
{
    if (items.size() > (std::size_t)std::numeric_limits<int>::max()) {
        throw std::length_error{"List extent"};
    }
    for (auto const& item : items) {
        validate_row_text(item.text);
    }
    items_ = std::move(items);
    ++items_revision_;
    click_row_.reset();
    hover_.reset();
    selection_ = {};
    offset_ = {};
    search_.clear();
    refresh();
}
void ListView::set_item(std::size_t index, ListItem item)
{
    if (index >= items_.size()) { throw std::out_of_range{"List item"}; }
    validate_row_text(item.text);
    items_[index] = std::move(item);
    ++items_revision_;
    click_row_.reset();
    if (!items_[index].enabled) {
        std::erase(selection_.rows, index);
        if (selection_.current == index) {
            selection_.current = row_target(index, items_.size(), 1,
                                            [&](auto i) { return items_[i].enabled; });
        }
        if (selection_.anchor == index) { selection_.anchor = selection_.current; }
    }
    refresh();
}
void ListView::insert_item(std::size_t index, ListItem item)
{
    if (index > items_.size()) { throw std::out_of_range{"List insertion"}; }
    validate_row_text(item.text);
    if (items_.size() == (std::size_t)std::numeric_limits<int>::max()) {
        throw std::length_error{"List extent"};
    }
    items_.insert(items_.begin() + (std::ptrdiff_t)index, std::move(item));
    ++items_revision_;
    click_row_.reset();
    for (auto& row : selection_.rows) {
        if (row >= index) { ++row; }
    }
    if (selection_.current && *selection_.current >= index) { ++*selection_.current; }
    if (selection_.anchor && *selection_.anchor >= index) { ++*selection_.anchor; }
    refresh();
}
void ListView::erase_item(std::size_t index)
{
    if (index >= items_.size()) { throw std::out_of_range{"List erasure"}; }
    items_.erase(items_.begin() + (std::ptrdiff_t)index);
    ++items_revision_;
    click_row_.reset();
    std::erase(selection_.rows, index);
    for (auto& row : selection_.rows) {
        if (row > index) { --row; }
    }
    for (auto* value : {&selection_.current, &selection_.anchor}) {
        if (*value == index) { value->reset(); }
        else if (*value && **value > index) {
            --**value;
        }
    }
    refresh();
}
auto ListView::measure(Area available, esc::WidthPolicy policy) const -> Area
{
    validate(available);
    if (items_.empty() && !empty_text.empty()) {
        validate_row_text(empty_text);
        return {measure_text(empty_text, available, TextWrap::None, policy).width, 1};
    }
    if (layouts_.size() < items_.size()) { layouts_.resize(items_.size()); }
    int width = 0;
    for (std::size_t i = 0; i < items_.size(); ++i) {
        layouts_[i].update(items_[i].text, available.width, TextWrap::None, policy);
        width = std::max(width, text_content_size(layouts_[i]).width);
    }
    if (indicator_width()) {
        if (width > std::numeric_limits<int>::max() - indicator_width())
            throw std::length_error{"List extent"};
        width += indicator_width();
    }
    return {width, (int)items_.size()};
}
void ListView::update_viewport()
{
    auto natural = measure(size(), glyph_width_policy());
    content_ = items_.empty() ? Area{} : natural;
    offset_ = {clamp_scroll(offset_.x, content_.width, size().width),
               clamp_scroll(offset_.y, content_.height, size().height)};
}
void ListView::refresh()
{
    request_layout();
    update_viewport();
    request_repaint();
    notify();
}
void ListView::on_arrange(Rect)
{
    update_viewport();
    notify();
}
void ListView::notify()
{
    auto lifetime = track(*this);
    if (notified_rows_ != selection_.rows) {
        notified_rows_ = selection_.rows;
        auto const value = notified_rows_;
        on_select(value);
    }
    auto* self = lifetime.try_get();
    if (!self) { return; }
    if (self->notified_current_ != self->selection_.current) {
        auto const value = self->selection_.current;
        self->notified_current_ = value;
        self->on_current(value);
    }
    self = lifetime.try_get();
    if (!self) { return; }
    auto const value = std::tuple{self->offset_, self->content_, self->size()};
    if (self->notified_scroll_ != value) {
        self->notified_scroll_ = value;
        self->on_scroll(std::get<0>(value), std::get<1>(value), std::get<2>(value));
    }
}
auto ListView::select(std::optional<std::size_t> row, SelectionAction action) -> bool
{
    auto old_offset = offset_;
    bool changed;
    if (row) {
        if (!multiple && action != SelectionAction::Move) {
            action = SelectionAction::Replace;
        }
        changed = select_rows(selection_, *row, items_.size(), action,
                              [&](auto i) { return items_[i].enabled; });
        if (items_[*row].enabled) {
            offset_.y =
                scroll_to_show(offset_.y, (int)*row, 1, content_.height, size().height);
        }
    }
    else {
        changed = selection_ != RowSelection{};
        selection_ = {};
    }
    changed |= old_offset != offset_;
    if (changed) {
        request_repaint();
        notify();
    }
    return changed;
}
auto ListView::select_all() -> bool
{
    if (!multiple) { return false; }
    auto rows = std::vector<std::size_t>{};
    for (std::size_t i = 0; i < items_.size(); ++i) {
        if (items_[i].enabled) { rows.push_back(i); }
    }
    if (rows == selection_.rows) { return false; }
    selection_.rows = std::move(rows);
    request_repaint();
    notify();
    return true;
}
auto ListView::activate(std::size_t row) -> bool
{
    if (row >= items_.size()) { throw std::out_of_range{"List activation"}; }
    if (!is_enabled() || !items_[row].enabled) { return false; }
    on_activate(row);
    return true;
}
auto ListView::set_scroll_offset(Point at) -> bool
{
    at = {clamp_scroll(at.x, content_.width, size().width),
          clamp_scroll(at.y, content_.height, size().height)};
    if (at == offset_) { return false; }
    offset_ = at;
    request_repaint();
    notify();
    return true;
}
auto ListView::paint(Canvas canvas) -> Cursor
{
    auto base =
        appearance(style | (is_focused() ? focused_style : blurred_style), role);
    fill(canvas, U' ' | base);
    if (items_.empty() && !empty_text.empty()) {
        auto ink = base;
        if (theme().styled && style.foreground == Color{default_color} &&
            !(is_focused() ? focused_style.foreground : blurred_style.foreground))
            ink.foreground = theme().palette.muted;
        ink = ink | empty_style;
        if (!is_enabled()) ink = ink | disabled_style;
        paint_row_text(canvas.subcanvas({}, {size().width, std::min(1, size().height)}),
                       empty_text, Align::Left, ink, empty_layout_);
    }
    if (layouts_.size() < items_.size()) { layouts_.resize(items_.size()); }
    auto const [first, last] = visible_rows(canvas, offset_.y, items_.size());
    auto const hovered = hovered_row();
    for (int row = first; row < last; ++row) {
        auto const& item = items_[(std::size_t)row];
        auto normal = base | item.style;
        if (row % 2) {
            if (striped && theme().styled)
                normal.background =
                    mix_color(normal.background, theme().palette.selection, .18);
            normal = normal | alternate_style;
        }
        if (hovered == (std::size_t)row && item.enabled) {
            if (theme().styled)
                normal.background =
                    mix_color(normal.background, theme().palette.selection, .4);
            normal = normal | hover_style;
        }
        auto const selected =
            std::ranges::binary_search(selection_.rows, (std::size_t)row);
        if (selected) {
            normal = theme_selection(normal, selected_style, theme(), is_focused());
            if (!is_focused()) normal = normal | inactive_selected_style;
        }
        if (is_focused() && selection_.current == (std::size_t)row) {
            normal = normal | current_style;
        }
        if (!is_enabled() || !item.enabled) normal = normal | disabled_style;
        auto line = canvas.subcanvas({0, row - offset_.y}, {size().width, 1});
        fill(line, U' ' | normal);
        auto gutter = indicator_width();
        if (gutter && (selection_.current == (std::size_t)row || selected)) {
            auto marker = normal;
            if (!indicator_style.foreground && !item.style.foreground &&
                style.foreground == Color{default_color})
                marker.foreground =
                    is_focused() ? theme().palette.accent : theme().palette.muted;
            marker = marker | indicator_style;
            if (!is_enabled() || !item.enabled) marker = marker | disabled_style;
            line.write({0, 0}, theme().ascii ? U">" : U"╺", marker);
        }
        paint_row_text(
            line.subcanvas({gutter, 0}, {std::max(0, size().width - gutter), 1})
                .subcanvas(
                    {-offset_.x, 0},
                    {std::max(0, std::max(content_.width, size().width) - gutter), 1}),
            item.text, Align::Left, normal, layouts_[(std::size_t)row]);
    }
    return {};
}
auto ListView::hovered_row() const -> std::optional<std::size_t>
{
    if (!hover_rows || !hover_ || !contains({{}, size()}, *hover_)) return {};
    auto row = (std::int64_t)hover_->y + offset_.y;
    return row < (std::int64_t)items_.size() ? std::optional{(std::size_t)row}
                                             : std::nullopt;
}
auto ListView::handle(MouseMove const& event) -> EventResponse
{
    auto before = hovered_row();
    hover_ = contains({{}, size()}, event.at) ? std::optional{event.at} : std::nullopt;
    auto changed = before != hovered_row();
    if (changed) request_repaint();
    return {.repaint = changed};
}
auto ListView::mouse_leave() -> EventResponse
{
    auto changed = hovered_row().has_value();
    hover_.reset();
    if (changed) request_repaint();
    return {.repaint = changed};
}
auto ListView::handle(KeyEvent const& event) -> EventResponse
{
    if (!is_enabled() || event.action == KeyAction::Release) { return {}; }
    if (event.matches(U'a', {.ctrl = true}, event.action)) {
        return {.handled = multiple, .repaint = select_all()};
    }
    if (auto steps = row_navigation(event, size().height)) {
        auto row = row_target(selection_.current, items_.size(), *steps,
                              [&](auto i) { return items_[i].enabled; });
        if (!row) { return {}; }
        auto action = event.modifiers.shift  ? SelectionAction::Extend
                      : event.modifiers.ctrl ? SelectionAction::Move
                                             : SelectionAction::Replace;
        auto changed = select(row, action);
        return {.handled = true, .repaint = changed};
    }
    if (event.matches(U' ', {.ctrl = true}, KeyAction::Press) && selection_.current) {
        return {.handled = true,
                .repaint = select(selection_.current, SelectionAction::Toggle)};
    }
    if (multiple && event.matches(U' ', {}, KeyAction::Press)) {
        auto row = row_target(selection_.current, items_.size(), 0,
                              [&](auto i) { return items_[i].enabled; });
        return row ? EventResponse{.handled = true,
                                   .repaint = select(row, SelectionAction::Toggle)}
                   : EventResponse{};
    }
    if (activates(event)) {
        auto row = row_target(selection_.current, items_.size(), 0,
                              [&](auto i) { return items_[i].enabled; });
        if (!row) { return {}; }
        auto lifetime = track(*this);
        auto revision = items_revision_;
        auto changed =
            select(row, multiple ? SelectionAction::Move : SelectionAction::Replace);
        if (auto* self = lifetime.try_get();
            self && self->items_revision_ == revision && self->current_row() == row) {
            self->activate(*row);
        }
        return {.handled = true, .repaint = changed};
    }
    auto text = input_text(event);
    if (text.empty()) { return {}; }
    auto now = std::chrono::steady_clock::now();
    if (now - search_time_ > std::chrono::milliseconds{800}) { search_.clear(); }
    search_time_ = now;
    if (search_.size() == 1 && text == search_) { search_.clear(); }
    search_ += text;
    auto fold = [](char32_t c) { return c >= U'A' && c <= U'Z' ? c + 32 : c; };
    for (std::size_t n = 0; n < items_.size(); ++n) {
        auto i =
            (selection_.current.value_or(items_.size() - 1) + 1 + n) % items_.size();
        auto label = esc::utf8_to_utf32(items_[i].text);
        if (items_[i].enabled && label.size() >= search_.size() &&
            std::equal(search_.begin(), search_.end(), label.begin(),
                       [&](auto a, auto b) { return fold(a) == fold(b); })) {
            return {.handled = true, .repaint = select(i)};
        }
    }
    return {.handled = true};
}
auto ListView::handle(MousePress const& event) -> EventResponse
{
    if (!is_enabled() || !activates(event) || !contains({{}, size()}, event.at)) {
        return {};
    }
    auto row = (std::size_t)(event.at.y + offset_.y);
    if (row >= items_.size() || !items_[row].enabled) { return {}; }
    auto now = std::chrono::steady_clock::now();
    auto double_click = click_row_ == row &&
                        now - click_time_ < std::chrono::milliseconds{400} &&
                        event.modifiers == Modifiers{};
    click_row_ = double_click ? std::nullopt : std::optional{row};
    click_time_ = now;
    auto action = event.modifiers.shift  ? SelectionAction::Extend
                  : event.modifiers.ctrl ? SelectionAction::Toggle
                                         : SelectionAction::Replace;
    auto lifetime = track(*this);
    auto revision = items_revision_;
    auto changed = select(row, action);
    if (auto* self = lifetime.try_get(); self && double_click &&
                                         self->items_revision_ == revision &&
                                         self->current_row() == row) {
        self->activate(row);
    }
    return {.handled = true, .repaint = changed};
}
auto ListView::handle(MouseWheel const& e) -> EventResponse
{
    auto changed =
        set_scroll_offset({scroll_by(offset_.x, e.dx, content_.width, size().width),
                           scroll_by(offset_.y, e.dy, content_.height, size().height)});
    return {.handled = changed, .repaint = changed};
}
}  // namespace ox
