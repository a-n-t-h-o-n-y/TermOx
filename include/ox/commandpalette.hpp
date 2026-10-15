#pragma once
#include <ox/command.hpp>
#include <ox/lineedit.hpp>
#include <ox/listview.hpp>
namespace ox {
/** Case-insensitive ASCII subsequence match; Unicode scalars match exactly. */
auto command_score(std::string_view query, std::string_view text)
    -> std::optional<std::size_t>;
/** Search + ranked choices. Compose inside Dialog/Popup for modal presentation. */
class CommandPalette : public Widget {
   public:
    using Widget::handle;
    LineEdit query{{.placeholder = "Find a command…"}};
    sl::Signal<void(std::size_t)> on_execute;
    CommandPalette();
    explicit CommandPalette(std::vector<std::shared_ptr<Command>>);
    auto commands() const -> std::span<std::shared_ptr<Command> const>
    { return commands_; }
    auto matches() const -> std::span<std::size_t const> { return matches_; }
    auto choices() const -> ListView const& { return choices_; }
    void refresh() override;
    void set_commands(std::vector<std::shared_ptr<Command>>);
    auto execute(std::size_t) -> bool;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto handle(KeyEvent const&) -> EventResponse override;
    auto get_children() -> zzz::Generator<Widget&> override
    {
        co_yield query;
        co_yield choices_;
    }
    auto get_children() const -> zzz::Generator<Widget const&> override
    {
        co_yield query;
        co_yield choices_;
    }

   protected:
    void on_arrange(Rect) override;

   private:
    std::vector<std::shared_ptr<Command>> commands_;
    std::vector<std::size_t> matches_;
    ListView choices_;
    void filter();
};
}  // namespace ox
