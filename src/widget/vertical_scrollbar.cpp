#include "widget/widgets/vertical_scrollbar.hpp"

#include "painter/color.hpp"
#include "widget/size_policy.hpp"

namespace cppurses {
Vertical_scrollbar::Vertical_scrollbar() {
    this->disable_cursor();
    this->width_policy.type(Size_policy::Fixed);
    this->width_policy.hint(1);
    this->height_policy.type(Size_policy::Expanding);

    up_button.height_policy.type(Size_policy::Fixed);
    up_button.height_policy.hint(1);

    middle.height_policy.type(Size_policy::Expanding);
    middle.brush().set_background(Color::Light_gray);

    down_button.height_policy.type(Size_policy::Fixed);
    down_button.height_policy.hint(1);
}

}  // namespace cppurses
