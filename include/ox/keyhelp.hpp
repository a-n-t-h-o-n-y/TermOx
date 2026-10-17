/** @file
 * Formatted keyboard shortcut hints for commands.
 */
#pragma once
#include <ox/command.hpp>
#include <ox/selection.hpp>
#include <ox/widget.hpp>
namespace ox {
/** Compact keyboard help; optional wrapping. Action bindings are scoped shortcuts. */
class KeyHelp : public Widget {
   public:
    /** Shared commands displayed and dispatched by this widget; call refresh() after
     * changing the list or command presentation.
     */
    std::vector<std::shared_ptr<Command>> bindings;
    /** Wrap help across multiple rows when true; call refresh() after changing it. */
    bool expanded = false;
    /** Base text style; explicit colors override theme defaults. */
    Style style;
    /** Retain shared commands, validate them, and install scoped shortcut dispatch. */
    explicit KeyHelp(std::vector<std::shared_ptr<Command>> = {});
    /** Dispatch press events to the first enabled matching action; releases and
     * unmatched keys have no effects.
     */
    auto handle(KeyEvent const&) -> EventResponse override;
    /** Measure command help with the current width policy and optional wrapping. */
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    /** Paint enabled command labels and their shortcuts inside the inherited clip. */
    auto paint(Canvas) -> Cursor override;

   private:
    TextLayout layout_;
    auto text(bool ascii) const -> std::string;
};
}  // namespace ox
