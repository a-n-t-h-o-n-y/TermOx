#pragma once
#include <ox/selection.hpp>
#include <ox/widget.hpp>
namespace ox {
struct KeyChord {
    Key key = KeyCode::Enter;
    Modifiers modifiers;
};
/** Help and dispatch share the same key definitions. Empty actions are help-only. */
struct Shortcut {
    std::vector<KeyChord> keys;
    std::string description;
    std::function<EventResponse()> action = {};
    bool enabled = true;
};
auto key_label(KeyChord const&, bool ascii = false) -> std::string;
auto dispatch_shortcuts(std::span<Shortcut const>, KeyEvent const&) -> EventResponse;
/** Compact keyboard help; optional wrapping. Action bindings are scoped shortcuts. */
class KeyHelp : public Widget {
   public:
    std::vector<Shortcut> bindings;
    bool expanded = false;
    Style style;
    explicit KeyHelp(std::vector<Shortcut> = {});
    auto handle(KeyEvent const&) -> EventResponse override;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas) -> Cursor override;

   private:
    TextLayout layout_;
    auto text(bool ascii) const -> std::string;
};
}  // namespace ox
