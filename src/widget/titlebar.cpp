#include <cppurses/widget/widgets/titlebar.hpp>

#include <utility>

#include <signals/signals.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widget_free_functions.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace cppurses {

Titlebar::Titlebar(const Glyph_string& title_)
    : title{this->make_child<Label>(title_)},
      exit_button{this->make_child<Push_button>(L"│✕ ")} {
    title.width_policy.type(Size_policy::Expanding);
    set_background(title, Color::White);
    set_foreground(title, Color::Black);
    exit_button.clicked.connect(System::quit);
    set_background(exit_button, Color::White);
    set_foreground(exit_button, Color::Black);
    exit_button.width_policy.type(Size_policy::Fixed);
    exit_button.width_policy.hint(3);
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(1);
    title.set_alignment(Alignment::Center);
}
void Titlebar::set_title(Glyph_string title_) {
    title.set_contents(std::move(title_));
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
