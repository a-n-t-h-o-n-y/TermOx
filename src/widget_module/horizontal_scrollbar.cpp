#include "widget_module/widgets/horizontal_scrollbar.hpp"
#include "painter_module/color.hpp"
#include "widget_module/size_policy.hpp"

namespace twf {

Horizontal_scrollbar::Horizontal_scrollbar() {
    this->disable_cursor();
    this->geometry().size_policy().vertical_policy = Size_policy::Fixed;
    this->geometry().set_height_hint(1);
    this->geometry().size_policy().horizontal_policy = Size_policy::Expanding;

    left_button.geometry().size_policy().horizontal_policy = Size_policy::Fixed;
    left_button.geometry().set_width_hint(1);

    middle.geometry().size_policy().horizontal_policy = Size_policy::Expanding;
    middle.brush().set_background(Color::Light_gray);

    right_button.geometry().size_policy().horizontal_policy = Size_policy::Fixed;
    right_button.geometry().set_width_hint(1);
}

}  // namespace twf
