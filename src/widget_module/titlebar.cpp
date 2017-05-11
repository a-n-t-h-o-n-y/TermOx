#include "widget_module/widgets/titlebar.hpp"

#include "painter_module/color.hpp"
#include "painter_module/glyph_string.hpp"
#include "system_module/system.hpp"
#include "widget_module/size_policy.hpp"
#include "widget_module/widgets/push_button.hpp"

namespace twf {

Titlebar::Titlebar(const Glyph_string& title)
    : title_{this->make_child<Push_button>(title)},
      exit_button_{this->make_child<Push_button>('X')} {
    title_.size_policy().horizontal_policy = Size_policy::Expanding;
    title_.set_background(Color::Light_blue);
    title_.set_foreground(Color::Black);
    exit_button_.clicked.connect(System::quit);
    exit_button_.set_background(Color::Orange);
    exit_button_.set_foreground(Color::Black);
    exit_button_.size_policy().horizontal_policy = Size_policy::Fixed;
    exit_button_.geometry().set_width_hint(3);
    this->size_policy().vertical_policy = Size_policy::Fixed;
    this->geometry().set_height_hint(1);
}

}  // namespace twf
