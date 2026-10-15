#pragma once
#include <ox/command.hpp>
#include <ox/selection.hpp>
#include <ox/widget.hpp>
namespace ox {
/** Compact keyboard help; optional wrapping. Action bindings are scoped shortcuts. */
class KeyHelp : public Widget {
   public:
    std::vector<std::shared_ptr<Command>> bindings;
    bool expanded = false;
    Style style;
    explicit KeyHelp(std::vector<std::shared_ptr<Command>> = {});
    auto handle(KeyEvent const&) -> EventResponse override;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas) -> Cursor override;

   private:
    TextLayout layout_;
    auto text(bool ascii) const -> std::string;
};
}  // namespace ox
