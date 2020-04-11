#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_EXAMPLES_INFO_BOX_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_EXAMPLES_INFO_BOX_HPP
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/button.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

namespace gol {

struct Examples_box : cppurses::layout::Vertical<cppurses::Button> {
    Examples_box();

    cppurses::Button& example_1   = this->make_child("Example 1");
    cppurses::Button& example_2   = this->make_child("Example 2");
    cppurses::Button& example_3   = this->make_child("Example 3");
    cppurses::Button& example_4   = this->make_child("Example 4");
    cppurses::Button& example_5   = this->make_child("Example 5");
    cppurses::Button& example_6   = this->make_child("Example 6");
    cppurses::Button& example_7   = this->make_child("Example 7");
    cppurses::Button& to_info_btn = this->make_child("Rulesets");
};

struct Info_box : cppurses::layout::Vertical<> {
    Info_box();

    cppurses::Text_display& info{
        this->make_child<cppurses::Text_display>("Rulesets go here")};
    cppurses::Button& to_examples_btn{
        this->make_child<cppurses::Button>("Patterns")};
};

/// Patterns_rulesets_box
struct Examples_info_box : public cppurses::layout::Stack<> {
    Examples_info_box();

    Examples_box& examples{this->make_page<Examples_box>()};
    Info_box& info{this->make_page<Info_box>()};
};

}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_EXAMPLES_INFO_BOX_HPP
