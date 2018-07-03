#include <cppurses/widget/widgets/vertical_scrollbar.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/textbox_base.hpp>
#include <cppurses/widget/widgets/textbox_base_slots.hpp>

namespace cppurses {

Vertical_scrollbar::Vertical_scrollbar() {
    this->initialize();
}

Vertical_scrollbar::Vertical_scrollbar(Textbox_base& tb) {
    this->initialize();
    up.connect(slot::scroll_up(tb, 1));
    down.connect(slot::scroll_down(tb, 1));
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
