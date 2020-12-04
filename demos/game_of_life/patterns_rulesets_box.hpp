#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_PATTERNS_RULESETS_BOX_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_PATTERNS_RULESETS_BOX_HPP
#include <string>
#include <utility>

#include <signals_light/signal.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widgets/button.hpp>
#include <cppurses/widget/widgets/button_list.hpp>

#include "colors.hpp"
#include "make_break.hpp"

namespace gol {

/// Custom Button.
class Thin_btn : public cppurses::Button {
   public:
    Thin_btn(cppurses::Glyph_string name) : Button{std::move(name)}
    {
        using namespace cppurses::pipe;
        *this | fixed_height(1uL) | cppurses::pipe::bg(color::Light_green) |
            cppurses::pipe::fg(color::Teal);
    }
};

/// A Button_list, line break, and goto Button in Vertical layout.
class Selection_page : public cppurses::layout::Vertical<> {
   private:
    using Selection_list = cppurses::Button_list<cppurses::layout::Vertical>;

   public:
    Selection_list& list = this->make_child<Selection_list>();
    Thin_btn& goto_btn;

   public:
    Selection_page(cppurses::Glyph_string goto_btn_name)
        : goto_btn{this->make_child<Thin_btn>(std::move(goto_btn_name))}
    {
        list.set_scrollbar_bg(color::Teal);
        list.set_scrollbar_fg(color::Light_green);
    }

   public:
    sl::Signal<void(std::wstring const& name)>& selection_made =
        list.button_pressed;

   public:
    void add_option(std::wstring const& name) { list.add_button(name); }
};

/// Stack of patterns and rules pages.
class Patterns_rulesets_box : public cppurses::layout::Stack<Selection_page> {
   public:
    // The String is for the GoTo Button
    Selection_page& patterns = this->make_page(L"Rulesets");
    Selection_page& rulesets = this->make_page(L"Patterns");

   public:
    Patterns_rulesets_box()
    {
        using namespace cppurses::pipe;
        *this | active_page(patterns_index) | fixed_height(10);

        patterns.goto_btn |
            on_press([this]() { this->set_active_page(rulesets_index); });

        rulesets.goto_btn |
            on_press([this]() { this->set_active_page(patterns_index); });
    }

   private:
    static auto constexpr patterns_index = 0uL;
    static auto constexpr rulesets_index = 1uL;
};

}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_PATTERNS_RULESETS_BOX_HPP
