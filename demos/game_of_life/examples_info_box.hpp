#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_EXAMPLES_INFO_BOX_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_EXAMPLES_INFO_BOX_HPP
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/button.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

namespace gol {

class Examples_box : public cppurses::layout::Vertical<cppurses::Button> {
   public:
    Child_t& example_1   = this->make_child("Example 1");
    Child_t& example_2   = this->make_child("Example 2");
    Child_t& example_3   = this->make_child("Example 3");
    Child_t& example_4   = this->make_child("Example 4");
    Child_t& example_5   = this->make_child("Example 5");
    Child_t& example_6   = this->make_child("Example 6");
    Child_t& example_7   = this->make_child("Example 7");
    Child_t& to_info_btn = this->make_child("Rulesets");

   public:
    Examples_box();
};

class Info_box : public cppurses::layout::Vertical<> {
   public:
    cppurses::Text_display& info =
        this->make_child<cppurses::Text_display>("Rulesets go here");

    cppurses::Button& to_examples_btn =
        this->make_child<cppurses::Button>("Patterns");

   public:
    Info_box();
};

/// Patterns_rulesets_box
class Examples_info_box : public cppurses::layout::Stack<> {
   public:
    Examples_box& examples = this->make_page<Examples_box>();
    Info_box& info         = this->make_page<Info_box>();

   public:
    Examples_info_box();
};

}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_EXAMPLES_INFO_BOX_HPP
