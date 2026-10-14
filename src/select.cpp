#include <algorithm>
#include <limits>
#include <ox/select.hpp>
namespace ox {
Select::Select() : Select{Options{}} {}
Select::Select(Options x)
    : Widget{FocusPolicy::None, SizePolicy::flex(), SizePolicy::fixed(1)},
      popup{ListView{{.items = std::move(x.items)}}},
      placeholder_{std::move(x.placeholder)}
{
    popup.anchor = track(static_cast<Widget&>(button));
    popup.placement = PopupPlacement::Below;
    button.label.align = Align::Left;
    Connection{.signal = button.on_activate,
               .slot = [](Select& self) { self.open(); }}(*this);
    Connection{.signal = popup.child.on_activate,
               .slot = [](std::size_t index, Select& self) {
                   auto lifetime = track(self);
                   self.popup.hide();
                   if (auto* current = lifetime.try_get()) { current->select(index); }
               }}(*this);
    select(x.selected);
    if (!x.selected) { button.label.text = placeholder_; }
}
auto Select::select(std::optional<std::size_t> index) -> bool
{
    if (index && *index >= items().size()) { throw std::out_of_range{"Select index"}; }
    if (index && !items()[*index].enabled) { return false; }
    if (index == selected_) { return false; }
    auto label = index ? items()[*index].text : placeholder_;
    selected_ = index;
    button.label.text = std::move(label);
    request_layout();
    request_repaint();
    on_change(index);
    return true;
}
void Select::set_items(std::vector<ListItem> items)
{
    auto lifetime = track(*this);
    popup.hide();
    auto* self = lifetime.try_get();
    if (!self) { return; }
    self->popup.child.set_items(std::move(items));
    self = lifetime.try_get();
    if (!self) { return; }
    auto changed = self->selected_.has_value();
    self->selected_.reset();
    self->button.label.text = self->placeholder_;
    self->refresh();
    if (changed) { self->on_change(std::nullopt); }
}
auto Select::open() -> bool
{
    if (!is_enabled() || items().empty() || popup.is_open()) { return false; }
    auto lifetime = track(*this);
    popup.refresh();
    auto* current = lifetime.try_get();
    if (!current) { return false; }
    current->popup.child.select(current->selected_);
    auto* self = lifetime.try_get();
    return self && self->popup.show();
}
auto Select::measure(Area available, esc::WidthPolicy policy) const -> Area
{
    auto width = measure_text(placeholder_, available, TextWrap::None, policy).width;
    for (auto const& item : items()) {
        width = std::max(
            width, measure_text(item.text, available, TextWrap::None, policy).width);
    }
    return {width, 1};
}
void Select::on_arrange(Rect) { arrange(button, {{}, size()}); }
auto Select::handle(KeyEvent const& e) -> EventResponse
{
    if (e.matches(KeyCode::ArrowDown, {.alt = true}, KeyAction::Press) ||
        e.matches(KeyCode::F4, {}, KeyAction::Press)) {
        return {.handled = true, .repaint = open()};
    }
    return {};
}
ComboBox::ComboBox() : ComboBox{Options{}} {}
ComboBox::ComboBox(Options x)
    : Widget{FocusPolicy::None, SizePolicy::flex(), SizePolicy::fixed(1)},
      editor{std::move(x.editor)},
      button{{.label = {.text = "▾"}}},
      popup{ListView{}}
{
    popup.modal = false;
    popup.anchor = track(static_cast<Widget&>(editor));
    popup.placement = PopupPlacement::Below;
    Connection{.signal = button.on_activate,
               .slot = [](ComboBox& self) { self.open(); }}(*this);
    Connection{.signal = editor.on_change,
               .slot = [](std::string const& value, ComboBox& self) {
                   self.edited(value);
               }}(*this);
    Connection{.signal = popup.on_close,
               .slot = [](ComboBox& self) { self.editor.request_focus(); }}(*this);
    Connection{.signal = popup.child.on_activate,
               .slot = [](std::size_t row, ComboBox& self) {
                   if (row >= self.matches_.size()) { return; }
                   auto index = self.matches_[row];
                   auto lifetime = track(self);
                   self.popup.hide();
                   if (auto* current = lifetime.try_get()) { current->select(index); }
               }}(*this);
    set_items(std::move(x.items));
}
void ComboBox::filter()
{
    auto values = std::vector<ListItem>{};
    auto matches = std::vector<std::size_t>{};
    auto query = esc::utf8_to_utf32(editor.text());
    auto fold = [](char32_t c) { return c >= U'A' && c <= U'Z' ? c + 32 : c; };
    for (auto& c : query) {
        c = fold(c);
    }
    for (std::size_t i = 0; i < items_.size(); ++i) {
        auto label = esc::utf8_to_utf32(items_[i].text);
        for (auto& c : label) {
            c = fold(c);
        }
        if (std::search(label.begin(), label.end(), query.begin(), query.end()) !=
                label.end() ||
            query.empty()) {
            values.push_back(items_[i]);
            matches.push_back(i);
        }
    }
    matches_ = std::move(matches);
    popup.child.set_items(std::move(values));
}
void ComboBox::edited(std::string const& value)
{
    auto lifetime = track(*this);
    auto changed = selected_.has_value() && items_[*selected_].text != value;
    if (changed) { selected_.reset(); }
    filter();
    auto* self = lifetime.try_get();
    if (!self) { return; }
    if (changed) { self->on_select(std::nullopt); }
    self = lifetime.try_get();
    if (self) { self->on_change(value); }
}
void ComboBox::set_items(std::vector<ListItem> items)
{
    for (auto const& item : items) {
        validate_row_text(item.text);
    }
    auto lifetime = track(*this);
    popup.hide();
    auto* self = lifetime.try_get();
    if (!self) { return; }
    self->items_ = std::move(items);
    auto changed = self->selected_.has_value();
    self->selected_.reset();
    self->filter();
    self = lifetime.try_get();
    if (self) {
        self->refresh();
        if (changed) { self->on_select(std::nullopt); }
    }
}
auto ComboBox::select(std::optional<std::size_t> index) -> bool
{
    if (index && *index >= items_.size()) { throw std::out_of_range{"Combo index"}; }
    if (index && !items_[*index].enabled) { return false; }
    auto changed = selected_ != index;
    auto value = index ? items_[*index].text : editor.text();
    auto lifetime = track(*this);
    selected_ = index;
    editor.set_text(value);
    auto* self = lifetime.try_get();
    if (!self) { return changed; }
    if (self->editor.text() != value || self->selected_ != index) { return false; }
    if (changed) { self->on_select(index); }
    return changed;
}
auto ComboBox::open() -> bool
{
    if (!is_enabled() || matches_.empty() || popup.is_open()) { return false; }
    auto lifetime = track(*this);
    popup.refresh();
    auto* current = lifetime.try_get();
    if (!current) { return false; }
    auto row = std::optional<std::size_t>{};
    if (current->selected_) {
        auto it = std::ranges::find(current->matches_, *current->selected_);
        if (it != current->matches_.end()) {
            row = (std::size_t)(it - current->matches_.begin());
        }
    }
    current->popup.child.select(row);
    current = lifetime.try_get();
    if (!current) { return false; }
    auto changed = current->popup.show();
    if (auto* self = lifetime.try_get()) { self->popup.child.request_focus(); }
    return changed;
}
auto ComboBox::measure(Area available, esc::WidthPolicy policy) const -> Area
{
    auto size = editor.measure(available, policy);
    if (size.width > std::numeric_limits<int>::max() - 2) {
        throw std::length_error{"Combo extent"};
    }
    return {size.width + 2, 1};
}
void ComboBox::on_arrange(Rect)
{
    auto width = std::min(2, size().width);
    arrange(editor, {{}, {size().width - width, size().height}});
    arrange(button, {{size().width - width, 0}, {width, size().height}});
}
auto ComboBox::handle(KeyEvent const& e) -> EventResponse
{
    if (e.matches(KeyCode::ArrowDown, {.alt = true}, KeyAction::Press) ||
        e.matches(KeyCode::F4, {}, KeyAction::Press)) {
        return {.handled = true, .repaint = open()};
    }
    return {};
}
}  // namespace ox
