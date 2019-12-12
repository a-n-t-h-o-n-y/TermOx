#include <cppurses/widget/widgets/vertical_scrollbar.hpp>

#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

void Vertical_scrollbar::initialize()
{
    this->width_policy.fixed(1);
    this->height_policy.type(Size_policy::Expanding);

    up_button.height_policy.fixed(1);

    middle.height_policy.type(Size_policy::Expanding);
    middle.brush.set_background(Color::Light_gray);

    down_button.height_policy.fixed(1);
}

}  // namespace cppurses
