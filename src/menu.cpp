#include <limits>
#include <ox/controls.hpp>
#include <ox/label.hpp>
#include <ox/menu.hpp>
namespace ox {
namespace {
void validate_entries(std::span<MenuItem const> items)
{
    for (auto const& item : items) {
        validate_row_text(item.text);
        validate_row_text(item.shortcut);
        validate_entries(item.children);
    }
}
auto display(std::span<MenuItem const> items) -> std::vector<ListItem>
{
    auto rows = std::vector<ListItem>{};
    for (auto const& item : items) {
        auto label =
            item.text.empty()
                ? "────────"
                : std::string{item.checked ? (*item.checked ? "✓ " : "  ") : "  "} +
                      item.text;
        if (!item.shortcut.empty()) { label += "  " + item.shortcut; }
        if (!item.children.empty()) { label += "  ▸"; }
        rows.push_back({std::move(label), item.enabled && !item.text.empty()});
    }
    return rows;
}
}  // namespace
Menu::Menu(std::vector<MenuItem> items) : ListView{}
{
    set_items(std::move(items));
    Connection{.signal = on_activate, .slot = [](std::size_t row, Menu& menu) {
                   menu.activate_item(row);
               }}(*this);
}
Menu::~Menu() = default;
Menu::Menu(Menu&&) = default;
auto Menu::operator=(Menu&&) -> Menu& = default;
void Menu::set_items(std::vector<MenuItem> items)
{
    validate_entries(items);
    auto rows = display(items);
    submenu_.reset();
    submenu_row_.reset();
    entries_ = std::move(items);
    ListView::set_items(std::move(rows));
}
void Menu::activate_item(std::size_t row)
{
    if (row >= entries_.size() || !entries_[row].enabled ||
        entries_[row].text.empty()) {
        return;
    }
    if (!entries_[row].children.empty()) {
        auto popup = std::make_unique<Popup<Menu>>(Menu{entries_[row].children});
        popup->modal = false;
        popup->placement = PopupPlacement::Right;
        popup->anchor = track(static_cast<Widget&>(*this));
        popup->offset = {0, (int)row - scroll_offset().y};
        Connection{.signal = popup->child.on_trigger,
                   .slot = [row](std::vector<std::size_t> const& path, Menu& parent) {
                       auto value = path;
                       value.insert(value.begin(), row);
                       parent.on_trigger(value);
                   }}(*this);
        Connection{.signal = popup->child.on_back,
                   .slot = [](auto& popup) { popup.hide(); }}(*popup);
        Connection{.signal = popup->on_close,
                   .slot = [](Menu& menu) { menu.request_focus(); }}(*this);
        Connection{
            .signal = popup->child.on_next,
            .slot = [](int direction, Menu& menu) { menu.on_next(direction); }}(*this);
        auto* surface = popup.get();
        submenu_ = std::move(popup);
        submenu_row_ = row;
        auto lifetime = track(*this);
        auto target = track(static_cast<Widget&>(surface->child));
        request_layout();
        surface->show();
        if (auto* self = lifetime.try_get(); self && target.valid()) {
            arrange(*surface, {{}, self->screen_size()});
            self = lifetime.try_get();
            if (self && target.valid()) { self->request_focus(target.get()); }
        }
        return;
    }
    auto action = entries_[row].action;
    auto value = std::vector<std::size_t>{row};
    on_trigger(value);
    if (action) { action(); }
}
auto Menu::handle(KeyEvent const& e) -> EventResponse
{
    if (e.matches(KeyCode::ArrowRight, {}, KeyAction::Press) && current_row() &&
        !entries_[*current_row()].children.empty()) {
        activate_item(*current_row());
        return {.handled = true, .repaint = true};
    }
    if (e.matches(KeyCode::ArrowRight, {}, KeyAction::Press) && !on_next.is_empty()) {
        on_next(1);
        return {.handled = true, .repaint = true};
    }
    if (e.matches(KeyCode::ArrowLeft, {}, KeyAction::Press)) {
        on_back();
        return {.handled = true, .repaint = true};
    }
    return ListView::handle(e);
}
auto Menu::handle(MousePress const& e) -> EventResponse
{
    auto lifetime = track(*this);
    auto const row =
        e.at.y >= 0 ? (std::size_t)(e.at.y + scroll_offset().y) : entries_.size();
    auto revision = items_revision();
    auto result = EventResponse{};
    if (is_enabled() && activates(e) && contains({{}, size()}, e.at) &&
        row < entries_.size() && entries_[row].enabled && !entries_[row].text.empty()) {
        result = {.handled = true, .repaint = select(row)};
    }
    if (auto* self = lifetime.try_get(); self && self->items_revision() == revision &&
                                         result.handled &&
                                         row < self->entries_.size()) {
        self->activate_item(row);
    }
    return result;
}
auto Menu::handle(MouseMove const& event) -> EventResponse
{
    if (!is_enabled() || !contains({{}, size()}, event.at)) { return {}; }
    auto row = (std::size_t)(event.at.y + scroll_offset().y);
    if (row >= entries_.size() || !entries_[row].enabled ||
        entries_[row].text.empty()) {
        return {};
    }
    if (current_row() == row && (!submenu_ || submenu_row_ == row)) {
        return {.handled = true};
    }
    auto lifetime = track(*this);
    auto revision = items_revision();
    auto changed = select(row);
    auto* self = lifetime.try_get();
    if (!self || self->items_revision() != revision) {
        return {.handled = true, .repaint = changed};
    }
    self->request_focus();
    self = lifetime.try_get();
    if (!self) { return {.handled = true, .repaint = true}; }
    if (!self->entries_[row].children.empty()) { self->activate_item(row); }
    else {
        self->close_submenu();
    }
    return {.handled = true, .repaint = true};
}
auto Menu::get_children() -> zzz::Generator<Widget&>
{
    if (submenu_) { co_yield *submenu_; }
}
auto Menu::get_children() const -> zzz::Generator<Widget const&>
{
    if (submenu_) { co_yield *submenu_; }
}
MenuBar::MenuBar(std::vector<MenuGroup> groups)
    : Widget{FocusPolicy::Strong, SizePolicy::flex(), SizePolicy::fixed(1)}
{
    auto lifetime = track(*this);
    shortcuts = [lifetime](KeyEvent const& event) {
        auto* self = lifetime.try_get();
        return self ? self->handle_shortcut(event) : EventResponse{};
    };
    buttons_.reserve(groups.size());
    popups_.reserve(groups.size());
    for (std::size_t i = 0; i < groups.size(); ++i) {
        validate_row_text(groups[i].text);
        auto& button = buttons_.emplace_back(
            Button::Options{.label = {.text = " " + groups[i].text + " "}});
        auto& popup = popups_.emplace_back(Menu{std::move(groups[i].items)});
        popup.anchor = track(static_cast<Widget&>(button));
        popup.placement = PopupPlacement::Below;
        auto bindings = popup.shortcuts;
        auto owner = track(*this);
        popup.shortcuts = [owner, bindings](KeyEvent const& event) {
            if (auto* bar = owner.try_get()) {
                if (event.matches(KeyCode::F10, {}, KeyAction::Press)) {
                    bar->close();
                    return EventResponse{.handled = true, .repaint = true};
                }
                if (event.modifiers == Modifiers{.alt = true}) {
                    auto effect = bar->handle_shortcut(event);
                    if (effect.handled) { return effect; }
                }
            }
            return bindings ? bindings(event) : EventResponse{};
        };
        Connection{.signal = button.on_activate,
                   .slot = [i](MenuBar& bar) { bar.open(i); }}(*this);
        Connection{.signal = popup.child.on_back, .slot = [i](MenuBar& bar) {
                       bar.open((i + bar.popups_.size() - 1) % bar.popups_.size());
                   }}(*this);
        Connection{
            .signal = popup.child.on_next, .slot = [i](int direction, MenuBar& bar) {
                auto count = (std::int64_t)bar.popups_.size();
                bar.open((std::size_t)(((std::int64_t)i + direction + count) % count));
            }}(*this);
        Connection{.signal = popup.child.on_trigger,
                   .slot = [i](std::vector<std::size_t> const& path, MenuBar& bar) {
                       auto value = path;
                       value.insert(value.begin(), i);
                       auto lifetime = track(bar);
                       bar.close();
                       if (auto* self = lifetime.try_get()) { self->on_trigger(value); }
                   }}(*this);
    }
}
void MenuBar::close()
{
    auto lifetime = track(*this);
    auto count = popups_.size();
    for (std::size_t i = 0; i < count; ++i) {
        auto* self = lifetime.try_get();
        if (!self) { return; }
        self->popups_[i].child.close_submenu();
        self->popups_[i].hide();
    }
}
auto MenuBar::open(std::size_t index) -> bool
{
    if (index >= popups_.size()) { throw std::out_of_range{"Menu index"}; }
    auto lifetime = track(*this);
    close();
    auto* self = lifetime.try_get();
    if (!self) { return false; }
    self->current_ = index;
    return self->popups_[index].show();
}
auto MenuBar::measure(Area available, esc::WidthPolicy policy) const -> Area
{
    validate(available);
    auto width = std::int64_t{0};
    for (auto const& button : buttons_) {
        width += button.measure(available, policy).width;
    }
    if (width > std::numeric_limits<int>::max()) {
        throw std::length_error{"Menu bar extent"};
    }
    return {(int)width, 1};
}
auto MenuBar::paint(Canvas canvas) -> Cursor
{
    fill(canvas, appearance({}));
    return {};
}
void MenuBar::on_arrange(Rect)
{
    auto x = 0;
    for (auto& button : buttons_) {
        auto width = std::min(button.measure(size(), glyph_width_policy()).width,
                              std::max(0, size().width - x));
        arrange(button, {{x, 0}, {width, std::min(1, size().height)}});
        x += width;
    }
}
auto MenuBar::handle_shortcut(KeyEvent const& event) -> EventResponse
{
    if (event.action != KeyAction::Press || buttons_.empty()) { return {}; }
    if (event.matches(KeyCode::F10, {}, KeyAction::Press)) {
        current_ = 0;
        buttons_[0].request_focus();
        return {.handled = true, .repaint = true};
    }
    if (event.modifiers != Modifiers{.alt = true}) { return {}; }
    for (std::size_t i = 0; i < buttons_.size(); ++i) {
        auto text = esc::utf8_to_utf32(buttons_[i].label.text);
        if (text.size() < 3) { continue; }
        auto c = text[1];
        if (c >= U'A' && c <= U'Z') { c += 32; }
        if (event.matches(c, {.alt = true}, KeyAction::Press)) {
            return {.handled = true, .repaint = open(i)};
        }
    }
    return {};
}
auto MenuBar::handle(KeyEvent const& e) -> EventResponse
{
    if (buttons_.empty() || e.action == KeyAction::Release) { return {}; }
    if (e.matches(KeyCode::ArrowDown, {}, KeyAction::Press)) {
        return {.handled = true, .repaint = open(current_.value_or(0))};
    }
    auto delta = e.matches(KeyCode::ArrowRight, {}, e.action)  ? 1
                 : e.matches(KeyCode::ArrowLeft, {}, e.action) ? -1
                                                               : 0;
    if (!delta) { return {}; }
    auto index =
        ((std::int64_t)current_.value_or(0) + delta + (std::int64_t)buttons_.size()) %
        (std::int64_t)buttons_.size();
    current_ = (std::size_t)index;
    buttons_[*current_].request_focus();
    return {.handled = true, .repaint = true};
}
auto MenuBar::get_children() -> zzz::Generator<Widget&>
{
    for (auto& b : buttons_) {
        co_yield b;
    }
    for (auto& p : popups_) {
        co_yield p;
    }
}
auto MenuBar::get_children() const -> zzz::Generator<Widget const&>
{
    for (auto const& b : buttons_) {
        co_yield b;
    }
    for (auto const& p : popups_) {
        co_yield p;
    }
}
}  // namespace ox
