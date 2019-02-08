#ifndef CPPURSES_DEMOS_FOCUS_FOCUS_WIDGETS_HPP
#define CPPURSES_DEMOS_FOCUS_FOCUS_WIDGETS_HPP
#include <cppurses/widget/focus_policy.hpp>

#include "focus_base.hpp"

namespace demos {
namespace focus {

struct Focus_none : Focus_base {
    Focus_none() : Focus_base{cppurses::Focus_policy::None} {}
};

struct Focus_click : Focus_base {
    Focus_click() : Focus_base{cppurses::Focus_policy::Click} {
        title_.focus_policy = cppurses::Focus_policy::Click;
    }
};

struct Focus_tab : Focus_base {
    Focus_tab() : Focus_base{cppurses::Focus_policy::Tab} {}
};

struct Focus_strong : Focus_base {
    Focus_strong() : Focus_base{cppurses::Focus_policy::Strong} {
        title_.focus_policy = cppurses::Focus_policy::Click;
    }
};

}  // namespace focus
}  // namespace demos
#endif  // CPPURSES_DEMOS_FOCUS_FOCUS_WIDGETS_HPP
