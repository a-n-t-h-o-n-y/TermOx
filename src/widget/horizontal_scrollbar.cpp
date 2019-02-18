#include <cppurses/widget/widgets/horizontal_scrollbar.hpp>

#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

Horizontal_scrollbar::Horizontal_scrollbar() {
    this->height_policy.fixed(1);
    this->width_policy.type(Size_policy::Expanding);

    left_button.width_policy.fixed(1);

    middle.width_policy.type(Size_policy::Expanding);
    middle.brush.set_background(Color::Light_gray);

    right_button.width_policy.fixed(1);
}
}  // namespace cppurses
