#include "widget/widgets/titlebar.hpp"
#include "painter/color.hpp"
#include "painter/glyph_string.hpp"
#include "system/system.hpp"
#include "widget/size_policy.hpp"
#include "widget/widgets/push_button.hpp"
#include "widget/widget.hpp"
#include <signals/signals.hpp>
#include <utility>

namespace cppurses {

Titlebar::Titlebar(const Glyph_string& title)
    : title_{this->make_child<Push_button>(title)},
      exit_button_{this->make_child<Push_button>("│✕ ")} {
    title_.width_policy.type(Size_policy::Expanding);
    set_background(title_, Color::White);
    set_foreground(title_, Color::Black);
    exit_button_.clicked.connect(System::quit);
    set_background(exit_button_, Color::White);
    set_foreground(exit_button_, Color::Black);
    exit_button_.width_policy.type(Size_policy::Fixed);
    exit_button_.width_policy.hint(3);
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(1);
}
void Titlebar::set_title(Glyph_string title) {
    title_.set_text(std::move(title));
}

namespace slot {

sig::Slot<void()> set_title(Titlebar& tb, const Glyph_string& title) {
    sig::Slot<void()> slot{[&tb, title] { tb.set_title(title); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void(Glyph_string)> set_title(Titlebar& tb) {
    sig::Slot<void(Glyph_string)> slot{
        [&tb](auto title) { tb.set_title(std::move(title)); }};
    slot.track(tb.destroyed);
    return slot;
}

}  // namespace slot

}  // namespace cppurses
