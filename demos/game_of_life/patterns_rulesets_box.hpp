#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_PATTERNS_RULESETS_BOX_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_PATTERNS_RULESETS_BOX_HPP
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widgets/button.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

#include <cppurses/widget/widgets/horizontal_scrollbar.hpp>

#include "colors.hpp"

namespace gol {

/// Layout Wrapper to make children scrollable and hooks up scrollbar.
template <typename Layout_t>
class Scrollable : public cppurses::layout::Horizontal<> {
   public:
   private:
    cppurses::HScrollbar& s = this->make_child<cppurses::HScrollbar>();
};

class Patterns_box : public cppurses::layout::Vertical<cppurses::Button> {
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
    Patterns_box()
    {
        using namespace cppurses::pipe;
        to_info_btn | fixed_height(1) | bg(color::Light_green) |
            fg(color::Teal);
    }
};

class Rulesets_box : public cppurses::layout::Vertical<> {
   public:
    cppurses::Text_display& info =
        this->make_child<cppurses::Text_display>("Rulesets go here");

    cppurses::Button& to_examples_btn =
        this->make_child<cppurses::Button>("Patterns");

   public:
    Rulesets_box()
    {
        using namespace cppurses::pipe;
        to_examples_btn | fixed_height(1uL) | bg(color::Light_green) |
            fg(color::Teal);
    }
};

class Patterns_rulesets_box : public cppurses::layout::Stack<> {
   public:
    Patterns_box& patterns = this->make_page<Patterns_box>();
    Rulesets_box& rulesets = this->make_page<Rulesets_box>();

   public:
    Patterns_rulesets_box()
    {
        using namespace cppurses::pipe;

        *this | active_page(0uL) | fixed_height(8uL);

        patterns.to_info_btn |
            on_press([this]() { this->set_active_page(info_index); });

        rulesets.to_examples_btn |
            on_press([this]() { this->set_active_page(examples_index); });
    }

   private:
    static auto constexpr examples_index = 0uL;
    static auto constexpr info_index     = 1uL;
};

}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_PATTERNS_RULESETS_BOX_HPP
