#include <cppurses/widget/widgets/cycle_stack.hpp>

#include <signals/signal.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/cycle_box.hpp>
#include <cppurses/widget/widgets/push_button.hpp>
#include <cppurses/widget/widgets/widget_stack.hpp>

namespace cppurses {

Cycle_stack::Top_row::Top_row() {
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(1);

    left_btn.width_policy.type(Size_policy::Fixed);
    left_btn.width_policy.hint(1);
    right_btn.width_policy.type(Size_policy::Fixed);
    right_btn.width_policy.hint(1);

    set_background_recursive(*this, Color::Light_gray);
    set_foreground_recursive(*this, Color::Black);

    left_btn.clicked.connect(slot::cycle_backward(cycle_box));
    right_btn.clicked.connect(slot::cycle_forward(cycle_box));

    cycle_box.brush.add_attributes(Attribute::Bold);
}

Cycle_stack::Cycle_stack() : top_row{this->make_child<Top_row>()} {}

void Cycle_stack::add_page(Glyph_string title, std::unique_ptr<Widget> widget) {
    sig::Signal<void()>& signal{top_row.cycle_box.add_option(std::move(title))};
    signal.connect(slot::set_active_page(stack, stack.size()));
    stack.add_page(std::move(widget));
    if (stack.size() == 1) {
        stack.set_active_page(0);
    }
}

}  // namespace cppurses
