#include <widget_module/widgets/vertical_scrollbar.hpp>

#include <painter_module/color.hpp>

namespace twf {
Vertical_scrollbar::Vertical_scrollbar() {
    this->set_cursor(false);
    this->geometry().size_policy().horizontal_policy = Size_policy::Fixed;
    this->geometry().set_width_hint(1);
    this->geometry().size_policy().vertical_policy = Size_policy::Expanding;

    up_button.geometry().size_policy().vertical_policy = Size_policy::Fixed;
    up_button.geometry().set_height_hint(1);

    // middle.geometry().size_policy().horizontal_policy = Size_policy::Fixed;
    // middle.geometry().set_width_hint(1);
    middle.geometry().size_policy().vertical_policy = Size_policy::Expanding;
    middle.brush().set_background(Color::Light_gray);

    down_button.geometry().size_policy().vertical_policy = Size_policy::Fixed;
    down_button.geometry().set_height_hint(1);
}

}  // namespace twf
