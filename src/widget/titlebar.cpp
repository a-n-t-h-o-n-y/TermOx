#include "widget/widgets/titlebar.hpp"

#include "painter/color.hpp"
#include "painter/glyph_string.hpp"
#include "system/system.hpp"
#include "widget/size_policy.hpp"
#include "widget/widgets/push_button.hpp"

namespace cppurses {

Titlebar::Titlebar(const Glyph_string& title)
    : title_{this->make_child<Push_button>(title)},
      exit_button_{this->make_child<Push_button>("│✕ ")} {
    title_.width_policy.type = Size_policy::Expanding;
    title_.set_background(Color::White);
    title_.set_foreground(Color::Black);
    exit_button_.clicked.connect(System::quit);
    exit_button_.set_background(Color::White);
    exit_button_.set_foreground(Color::Black);
    exit_button_.width_policy.type = Size_policy::Fixed;
    exit_button_.width_policy.hint = 3;
    this->height_policy.type = Size_policy::Fixed;
    this->height_policy.hint = 1;
}

}  // namespace cppurses
