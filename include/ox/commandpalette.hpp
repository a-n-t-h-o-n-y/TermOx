/** @file
 * Searchable command menus with keyboard activation.
 */
#pragma once
#include <ox/command.hpp>
#include <ox/lineedit.hpp>
#include <ox/listview.hpp>
namespace ox {
/** Score a subsequence match by skipped scalars; smaller scores rank first, and nullopt
 * means no match. Ignore query spaces, fold ASCII case, and match other Unicode scalars
 * exactly.
 */
auto command_score(std::string_view query, std::string_view text)
    -> std::optional<std::size_t>;
/** Search + ranked choices. Compose inside Dialog/Popup for modal presentation. */
class CommandPalette : public Widget {
   public:
    using Widget::handle;
    /** Owned single-line command search editor. */
    LineEdit query{{.placeholder = "Find a command…"}};
    /** Emits the original command index before its snapshotted action is invoked. */
    sl::Signal<void(std::size_t)> on_execute;
    /** Create an empty ranked command search with an owned query editor. */
    CommandPalette();
    /** Retain and validate shared commands, caching search text independently of the query. */
    explicit CommandPalette(std::vector<std::shared_ptr<Command>>);
    /** Borrow the original shared command sequence until set_commands(). */
    auto commands() const -> std::span<std::shared_ptr<Command> const>
    { return commands_; }
    /** Borrow ranked original-command indices until query or command refresh. */
    auto matches() const -> std::span<std::size_t const> { return matches_; }
    /** Inspect the current ranked result list; mutation remains with the palette. */
    auto choices() const -> ListView const& { return choices_; }
    /** Rebuild cached search text after editing shared commands or presentation. */
    void refresh() override;
    /** Validate and replace shared commands, rebuild search caches, and refilter the
     * current query.
     */
    void set_commands(std::vector<std::shared_ptr<Command>>);
    /** Invoke an enabled command by its original index, emitting on_execute before its
     * copied action; return false if disabled or actionless. Invalid indices throw.
     */
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
    std::vector<std::u32string> search_;
    void filter();
};
}  // namespace ox
