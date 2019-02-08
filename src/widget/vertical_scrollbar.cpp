#include <cppurses/widget/widgets/vertical_scrollbar.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widget_free_functions.hpp>

namespace cppurses {

Vertical_scrollbar::Vertical_scrollbar() {
    this->initialize();
}

void Vertical_scrollbar::initialize() {
    this->width_policy.type(Size_policy::Fixed);
    this->width_policy.hint(1);
    this->height_policy.type(Size_policy::Expanding);

    up_button.height_policy.type(Size_policy::Fixed);
    up_button.height_policy.hint(1);

    middle.height_policy.type(Size_policy::Expanding);
    set_background(middle, Color::Light_gray);

    down_button.height_policy.type(Size_policy::Fixed);
    down_button.height_policy.hint(1);
}

}  // namespace cppurses
