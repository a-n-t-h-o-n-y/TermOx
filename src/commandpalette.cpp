#include <limits>
#include <ox/commandpalette.hpp>
namespace ox {
namespace {
auto folded(std::string_view text) -> std::u32string
{
    auto result = esc::utf8_to_utf32(text);
    for (auto& c : result) {
        if (c >= U'A' && c <= U'Z') { c += 32; }
    }
    return result;
}
auto score(std::u32string_view q, std::u32string_view text)
    -> std::optional<std::size_t>
{
    auto at = std::size_t{0}, result = std::size_t{0};
    for (auto c : q) {
        if (c == U' ') { continue; }
        auto begin = at;
        while (at < text.size() && text[at] != c) {
            ++at;
        }
        if (at == text.size()) { return {}; }
        result += at - begin;
        ++at;
    }
    return result;
}
auto prepare_search(std::span<std::shared_ptr<Command> const> commands)
    -> std::vector<std::u32string>
{
    auto search = std::vector<std::u32string>{};
    search.reserve(commands.size());
    for (auto const& value : commands) {
        if (!value) { throw std::invalid_argument{"Null palette command"}; }
        validate(*value);
        search.push_back(folded(value->text + " " + value->keywords));
    }
    return search;
}
}  // namespace
auto command_score(std::string_view query, std::string_view text)
    -> std::optional<std::size_t>
{ return score(folded(query), folded(text)); }
CommandPalette::CommandPalette()
    : CommandPalette{std::vector<std::shared_ptr<Command>>{}}
{}
CommandPalette::CommandPalette(std::vector<std::shared_ptr<Command>> commands)
    : Widget{}
{
    Connection{.signal = query.on_change,
               .slot = [](auto const&, CommandPalette& self) { self.filter(); }}(*this);
    Connection{.signal = query.on_submit,
               .slot = [](auto const&, CommandPalette& self) {
                   auto row = self.choices_.current_row();
                   if (row && *row < self.matches_.size())
                       self.execute(self.matches_[*row]);
               }}(*this);
    Connection{.signal = choices_.on_activate,
               .slot = [](std::size_t row, CommandPalette& self) {
                   if (row < self.matches_.size()) self.execute(self.matches_[row]);
               }}(*this);
    set_commands(std::move(commands));
}
void CommandPalette::set_commands(std::vector<std::shared_ptr<Command>> commands)
{
    auto search = prepare_search(commands);
    commands_ = std::move(commands);
    search_ = std::move(search);
    filter();
}
void CommandPalette::refresh()
{
    search_ = prepare_search(commands_);
    auto lifetime = track(*this);
    filter();
    if (auto* self = lifetime.try_get()) {
        self->choices_.refresh();
        if ((self = lifetime.try_get())) { self->Widget::refresh(); }
    }
}
void CommandPalette::filter()
{
    auto current = choices_.current_row();
    auto selected = current && *current < matches_.size()
                        ? std::optional{matches_[*current]}
                        : std::nullopt;
    auto query_text = folded(query.text());
    auto ranked = std::vector<std::pair<std::size_t, std::size_t>>{};
    ranked.reserve(commands_.size());
    for (std::size_t i = 0; i < search_.size(); ++i) {
        if (auto value = score(query_text, search_[i])) {
            ranked.push_back({*value, i});
        }
    }
    auto less = [](auto a, auto b) { return a.first < b.first; };
    if (!std::is_sorted(ranked.begin(), ranked.end(), less)) {
        std::stable_sort(ranked.begin(), ranked.end(), less);
    }
    auto unchanged = ranked.size() == matches_.size();
    for (std::size_t row = 0; unchanged && row < ranked.size(); ++row) {
        auto index = ranked[row].second;
        unchanged = index == matches_[row] &&
                    choices_.items()[row].text == commands_[index]->text &&
                    choices_.items()[row].enabled == commands_[index]->enabled;
    }
    if (unchanged) { return; }
    auto items = std::vector<ListItem>{};
    items.reserve(ranked.size());
    matches_.clear();
    matches_.reserve(ranked.size());
    for (auto [score, i] : ranked) {
        matches_.push_back(i);
        items.push_back({commands_[i]->text, commands_[i]->enabled});
    }
    auto lifetime = track(*this);
    choices_.set_items(std::move(items));
    auto* self = lifetime.try_get();
    if (!self) return;
    auto target = std::optional<std::size_t>{};
    if (selected) {
        auto it = std::find(self->matches_.begin(), self->matches_.end(), *selected);
        if (it != self->matches_.end())
            target = (std::size_t)(it - self->matches_.begin());
    }
    if (target && !self->commands_[self->matches_[*target]]->enabled) {
        target.reset();
    }
    if (!target)
        target = row_target({}, self->matches_.size(), 1, [&](std::size_t i) {
            return self->commands_[self->matches_[i]]->enabled;
        });
    self->choices_.select(target);
    if ((self = lifetime.try_get())) self->Widget::refresh();
}
auto CommandPalette::execute(std::size_t index) -> bool
{
    if (!is_enabled() || index >= commands_.size() || !commands_[index]->enabled)
        return false;
    auto action = commands_[index]->action;
    auto respond = effect_handler();
    on_execute(index);
    if (action) {
        auto response = combine({.handled = true}, action());
        if (respond) { respond(response); }
    }
    return true;
}
auto CommandPalette::handle(KeyEvent const& e) -> EventResponse
{
    if (!is_enabled() || !query.is_focused()) return {};
    if (row_navigation(e, std::max(1, choices_.size().height)))
        return choices_.handle(e);
    return {};
}
auto CommandPalette::measure(Area a, esc::WidthPolicy p) const -> Area
{
    validate(a);
    auto q = query.measure(a, p), c = choices_.measure(a, p);
    if (c.height == std::numeric_limits<int>::max())
        throw std::length_error{"Palette height"};
    return {std::max(q.width, c.width), c.height + 1};
}
void CommandPalette::on_arrange(Rect)
{
    auto lifetime = track(*this);
    arrange(query, {{}, {size().width, std::min(1, size().height)}});
    if (auto* self = lifetime.try_get())
        arrange(self->choices_,
                {{0, std::min(1, self->size().height)},
                 {self->size().width, std::max(0, self->size().height - 1)}});
}
}  // namespace ox
