#ifndef TERMOX_DEMOS_TWO_LISTS_HPP
#define TERMOX_DEMOS_TWO_LISTS_HPP
#include <string>
#include <utility>

#include <termox/termox.hpp>
#include "termox/widget/pipe.hpp"

namespace demo {

using namespace ox;

using Check      = Selectable<HCheckbox_label>;
using Check_list = Array<layout::Selecting<layout::Vertical<Check>>, 15>;

struct My_check_list
    : Bordered<ox::VPair<ox::Passive<Check_list>, ox::Widget>> {
   public:
    Check_list& list = this->wrapped.first;

   public:
    My_check_list()
    {
        using namespace ox;
        using namespace ox::pipe;

        list.set_increment_selection_keys({Key::Arrow_down, Key::j});
        list.set_decrement_selection_keys({Key::Arrow_up, Key::k});
        list.set_increment_scroll_keys({Key::J});
        list.set_decrement_scroll_keys({Key::K});

        list | children() | for_each([i = 0uL](auto& w) mutable {
            w.label.set_text("number: " + std::to_string(i++));
        });
        list | bind_key(Key::Enter,
                        [](auto& w) { w.selected_child().checkbox.toggle(); });
    }
};

struct Two_lists : VArray<My_check_list, 2> {
   public:
    Two_lists()
    {
        this->get<0>().set_name("Check list 0");
        this->get<0>().list.set_name("list 0");
        this->get<1>().set_name("Check list 1");  // focus is set here, and then
                                                  // forwarded somewhere?
        this->get<1>().list.set_name("list 1");
        *this | pipe::direct_focus() | pipe::forward_focus(this->get<0>().list);
        this->foo();
    }

   public:
    void foo() const
    {
        (void)this->find_child_if(
            [](My_check_list const& l) { return l.name().empty(); });
    }
};

using Settings_box = ox::Tuple<ox::layout::Vertical<>,
                               ox::Button,
                               ox::HLabel,
                               ox::Checkbox1,
                               ox::Cycle_box,
                               ox::Push_button>;

class Idea : public Settings_box {
   public:
    Idea()
    {
        using namespace ox;
        using namespace ox::pipe;

        this->get<0>().set_text(U"I'm a BUTTON");
        this->get<0>() | on_press([this] { this->get<2>().toggle(); });

        this->get<1>().set_text(U"I'm a LABEL");
        this->get<2>().check();

        this->get<3>() | bg(Color::Dark_gray);

        this->get<3>().add_option(U"Option BLACK").connect([this] {
            this->get<0>() | bg(Color::Black);
        });
        this->get<3>().add_option(U"Option RED").connect([this] {
            this->get<0>() | bg(Color::Red);
        });
        this->get<3>().add_option(U"Option GREEN").connect([this] {
            this->get<0>() | bg(Color::Green);
        });

        auto const s = std::to_string(sizeof(Glyph));
        this->get<4>().set_text(s);
        this->get<4>().set_pressed_color(Color::Violet);
        this->get<4>().set_released_color(Color::Blue);
    }
};

}  // namespace demo
#endif  // TERMOX_DEMOS_TWO_LISTS_HPP
