#ifndef CPPURSES_DEMOS_FOCUS_FOCUS_DEMO_HPP
#define CPPURSES_DEMOS_FOCUS_FOCUS_DEMO_HPP
#include <cppurses/widget/layouts/horizontal_layout.hpp>
#include <cppurses/widget/layouts/vertical_layout.hpp>

#include "focus_widgets.hpp"

namespace demos {
namespace focus {

struct Left_side : cppurses::Vertical_layout {
    Focus_tab& one{this->make_child<Focus_tab>()};
};

struct Center : cppurses::Vertical_layout {
    Focus_strong& one{this->make_child<Focus_strong>()};
    Focus_none& two{this->make_child<Focus_none>()};
};

struct Right_side : cppurses::Vertical_layout {
    Focus_click& one{this->make_child<Focus_click>()};
};

class Focus_demo : public cppurses::Horizontal_layout {
    Left_side& left_side{this->make_child<Left_side>()};
    Center& center{this->make_child<Center>()};
    Right_side& right_side{this->make_child<Right_side>()};
};

}  // namespace focus
}  // namespace demos
#endif  // CPPURSES_DEMOS_FOCUS_FOCUS_DEMO_HPP
