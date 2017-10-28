#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal_layout.hpp>
#include <cppurses/widget/widgets/cycle_box.hpp>
#include <cppurses/widget/widgets/cycle_stack.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

#include <signals/signal.hpp>

namespace cppurses {

struct Cycle_stack::Top_row : public Horizontal_layout {
    Top_row() {
        this->height_policy.type(Size_policy::Fixed);
        this->height_policy.hint(1);

        left_btn.width_policy.type(Size_policy::Fixed);
        left_btn.width_policy.hint(1);
        right_btn.width_policy.type(Size_policy::Fixed);
        right_btn.width_policy.hint(1);

        // set_background(cycle_box, Color::Orange);

        left_btn.clicked.connect(slot::cycle_backward(cycle_box));
        right_btn.clicked.connect(slot::cycle_forward(cycle_box));
    }
    Push_button& left_btn{this->make_child<Push_button>("⏴")};
    Cycle_box& cycle_box{this->make_child<Cycle_box>()};
    Push_button& right_btn{this->make_child<Push_button>("⏵")};
};

Cycle_stack::Cycle_stack() : top_row{this->make_child<Top_row>()} {}

void Cycle_stack::add_page(Glyph_string title, std::unique_ptr<Widget> widget) {
    sig::Signal<void()>& signal{top_row.cycle_box.add_option(std::move(title))};
    signal.connect(slot::set_active_page(stack, stack.size()));
    stack.add_page(std::move(widget));
}

}  // namespace cppurses
