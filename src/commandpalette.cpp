#include <limits>
#include <ox/commandpalette.hpp>
namespace ox {
auto command_score(std::string_view query, std::string_view text)
    -> std::optional<std::size_t>
{
    auto q = esc::utf8_to_utf32(query), t = esc::utf8_to_utf32(text);
    auto fold = [](char32_t c) { return c >= U'A' && c <= U'Z' ? c + 32 : c; };
    auto at = std::size_t{0}, score = std::size_t{0};
    for (auto c : q) {
        if (c == U' ') continue;
        auto begin = at;
        while (at < t.size() && fold(t[at]) != fold(c))
            ++at;
        if (at == t.size()) return {};
        score += at - begin;
        ++at;
    }
    return score;
}
CommandPalette::CommandPalette() : CommandPalette{std::vector<Command>{}} {}
CommandPalette::CommandPalette(std::vector<Command> commands) : Widget{}
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
void CommandPalette::set_commands(std::vector<Command> commands)
{
    for (auto const& c : commands) {
        validate_row_text(c.text);
        validate_row_text(c.keywords);
    }
    commands_ = std::move(commands);
    filter();
}
void CommandPalette::filter()
{
    auto current = choices_.current_row();
    auto selected = current && *current < matches_.size()
                        ? std::optional{matches_[*current]}
                        : std::nullopt;
    auto ranked = std::vector<std::pair<std::size_t, std::size_t>>{};
    for (std::size_t i = 0; i < commands_.size(); ++i)
        if (auto s = command_score(query.text(),
                                   commands_[i].text + " " + commands_[i].keywords))
            ranked.push_back({*s, i});
    std::stable_sort(ranked.begin(), ranked.end(),
                     [](auto a, auto b) { return a.first < b.first; });
    auto items = std::vector<ListItem>{};
    matches_.clear();
    for (auto [score, i] : ranked) {
        matches_.push_back(i);
        items.push_back({commands_[i].text, commands_[i].enabled});
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
    if (target && !self->commands_[self->matches_[*target]].enabled) { target.reset(); }
    if (!target)
        target = row_target({}, self->matches_.size(), 1, [&](std::size_t i) {
            return self->commands_[self->matches_[i]].enabled;
        });
    self->choices_.select(target);
    if ((self = lifetime.try_get())) self->refresh();
}
auto CommandPalette::execute(std::size_t index) -> bool
{
    if (!is_enabled() || index >= commands_.size() || !commands_[index].enabled)
        return false;
    auto action = commands_[index].action;
    on_execute(index);
    if (action) action();
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
