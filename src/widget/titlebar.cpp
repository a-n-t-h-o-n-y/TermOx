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
    title_.size_policy().horizontal_policy = Size_policy::Expanding;
    title_.set_background(Color::White);
    title_.set_foreground(Color::Black);
    exit_button_.clicked.connect(System::quit);
    exit_button_.set_background(Color::White);
    exit_button_.set_foreground(Color::Black);
    exit_button_.size_policy().horizontal_policy = Size_policy::Fixed;
    exit_button_.geometry().set_width_hint(3);
    this->size_policy().vertical_policy = Size_policy::Fixed;
    this->geometry().set_height_hint(1);
}

}  // namespace cppurses
