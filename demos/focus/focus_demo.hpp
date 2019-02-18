#ifndef CPPURSES_DEMOS_FOCUS_FOCUS_DEMO_HPP
#define CPPURSES_DEMOS_FOCUS_FOCUS_DEMO_HPP
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>

#include "focus_widgets.hpp"

namespace demos {
namespace focus {

struct Sub_boxes_one : cppurses::layout::Horizontal {
    Focus_strong& one{this->make_child<Focus_strong>()};
    Focus_tab& two{this->make_child<Focus_tab>()};
};

struct Sub_boxes_two : cppurses::layout::Vertical {
    Focus_none& one{this->make_child<Focus_none>()};
    Focus_tab& two{this->make_child<Focus_tab>()};
};

struct Sub_boxes_three : cppurses::layout::Horizontal {
    Focus_strong& one{this->make_child<Focus_strong>()};
    Sub_boxes_two& two{this->make_child<Sub_boxes_two>()};
    Focus_tab& three{this->make_child<Focus_tab>()};
};

struct Left_side : cppurses::layout::Vertical {
    Left_side() { one.height_policy.stretch(3); }
    Focus_tab& one{this->make_child<Focus_tab>()};
    Sub_boxes_one& two{this->make_child<Sub_boxes_one>()};
};

struct Center : cppurses::layout::Vertical {
    Focus_strong& one{this->make_child<Focus_strong>()};
    Focus_none& two{this->make_child<Focus_none>()};
};

struct Right_side : cppurses::layout::Vertical {
    Right_side() { two.height_policy.stretch(2); }
    Focus_click& one{this->make_child<Focus_click>()};
    Sub_boxes_three& two{this->make_child<Sub_boxes_three>()};
    Focus_strong& three{this->make_child<Focus_strong>()};
};

class Focus_demo : public cppurses::layout::Horizontal {
    Left_side& left_side{this->make_child<Left_side>()};
    Center& center{this->make_child<Center>()};
    Right_side& right_side{this->make_child<Right_side>()};
};

}  // namespace focus
}  // namespace demos
#endif  // CPPURSES_DEMOS_FOCUS_FOCUS_DEMO_HPP
