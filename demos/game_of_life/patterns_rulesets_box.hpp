#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_PATTERNS_RULESETS_BOX_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_PATTERNS_RULESETS_BOX_HPP
#include <string>
#include <utility>

#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widgets/button.hpp>
#include <cppurses/widget/widgets/scrollbar.hpp>
#include <cppurses/widget/widgets/text_display.hpp>
#include <cppurses/widget/widgets/textbox.hpp>

#include "colors.hpp"
#include "make_break.hpp"

namespace gol {

class Thin_btn : public cppurses::Button {
   public:
    Thin_btn(cppurses::Glyph_string text) : Button{std::move(text)}
    {
        *this | cppurses::pipe::fixed_height(1uL);
    }
};

class Patterns_list : public cppurses::layout::Horizontal<> {
    class Patterns_list_impl : public cppurses::layout::Vertical<Thin_btn> {
       public:
        sig::Signal<void(std::wstring const& name)> pattern_chosen;

       public:
        void add_pattern(std::wstring const& name)
        {
            this->Layout::append(std::make_unique<Thin_btn>(name))
                .pressed.connect([this, name] { pattern_chosen(name); });
        }
    };

   public:
    cppurses::VScrollbar& scrollbar = this->make_child<cppurses::VScrollbar>();

    Patterns_list_impl& list = this->make_child<Patterns_list_impl>();

   public:
    sig::Signal<void(std::wstring const& name)>& pattern_chosen =
        list.pattern_chosen;

   public:
    Patterns_list() { link(scrollbar, list); }

   public:
    void add_pattern(std::wstring const& name) { list.add_pattern(name); }
};

class Patterns_box : public cppurses::layout::Vertical<> {
   public:
    Patterns_list& list         = this->make_child<Patterns_list>();
    Widget& break_              = this->append(make_break());
    Thin_btn& goto_rulesets_btn = this->make_child<Thin_btn>(L"Rulesets") |
                                  cppurses::pipe::bg(color::Light_green) |
                                  cppurses::pipe::fg(color::Teal);

   public:
    sig::Signal<void(std::wstring const& name)>& pattern_chosen =
        list.pattern_chosen;

   public:
    void add_pattern(std::wstring const& name) { list.add_pattern(name); }
};

class Rules_list {};
class Rulesets_box : public cppurses::layout::Vertical<> {
   public:
    cppurses::Text_display& td =
        this->make_child<cppurses::Text_display>("Rulesets here.");

    cppurses::HScrollbar& bar = this->make_child<cppurses::HScrollbar>();

    Widget& break_ = this->append(make_break());

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

        *this | active_page(patterns_index) | fixed_height(16uL);

        patterns.goto_rulesets_btn |
            on_press([this]() { this->set_active_page(rulesets_index); });

        rulesets.to_examples_btn |
            on_press([this]() { this->set_active_page(patterns_index); });
    }

   private:
    static auto constexpr patterns_index = 0uL;
    static auto constexpr rulesets_index = 1uL;
};

}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_PATTERNS_RULESETS_BOX_HPP
