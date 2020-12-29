#ifndef TERMOX_DEMOS_COMPOSITES_COMPOSITES_HPP
#define TERMOX_DEMOS_COMPOSITES_COMPOSITES_HPP
#include <string>
#include <utility>

#include <termox/system/key.hpp>
#include <termox/widget/array.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/selecting.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pair.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/tuple.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/widgets/button.hpp>
#include <termox/widget/widgets/checkbox.hpp>
#include <termox/widget/widgets/cycle_box.hpp>
#include <termox/widget/widgets/selectable.hpp>
#include <termox/widget/widgets/textbox.hpp>

namespace comp {

using Btns = ox::Array<ox::layout::Horizontal<ox::Button>, 2>;

using App = ox::Tuple<ox::layout::Vertical<>, ox::Textbox, Btns>;

struct Composites_old : App {
    Composites_old()
    {
        auto& btns = this->get<1>();
        auto& save = btns.get<0>();
        auto& load = btns.get<1>();
        auto& txbx = this->get<0>();

        using namespace ox;
        using namespace ox::pipe;
        btns | fixed_height(1);
        // save | label("Save") | bg(Color::Light_blue);
        // load | label("Load") | bg(Color::Light_green);
        txbx | bg(Color::White) | fg(Color::Black);

        // *this | children() | fixed_height(4);
        *this | descendants() | bg(Color::Blue);

        save | bg(Color::Background);
        txbx | fg(Color::Foreground);

        // save | add_trait(Trait::Bold, Trait::Underline);
        // save.brush.add_traits(Trait::Bold, Trait::Underline);
        save | Trait::Bold | Trait::Underline;
        load | Trait::Bold | Trait::Underline;

        // save | Trait::Bold | Trait::Underline;
        // save | Trait::Bold | Trait::Underline;

        // save | remove_traits(Trait::Underline);
        save | discard(Trait::Underline);
        save | clear_traits();
        save | on_enable([&txbx]() { txbx.set_contents("Save Enabled"); });
        save | on_child_added([](Widget& w) { w.border.enable(); });

        load | on_move([&txbx](auto const& n, auto const&) {
            txbx.set_contents(std::to_string(n.x) + " " + std::to_string(n.y));
        });
        load | on_resize([&txbx](auto const& n, auto const&) {
            txbx.set_contents(std::to_string(n.width) + " " +
                              std::to_string(n.height));
        });

        load | on_mouse_press([&](auto) { load | bg(Color::Light_gray); });
        load | on_mouse_release([&](auto) { load | bg(Color::Dark_gray); });
        txbx | on_key_press([&](auto) { txbx | bg(Color::Light_blue); });

        using namespace std::literals;
        save | animate(34ms) | wallpaper(L'X') | wallpaper(std::nullopt);
        load | wallpaper({L'-', fg(Color::Red)});
        load | wallpaper_without_brush() | wallpaper_with_brush();

        load | show_cursor() | put_cursor({2, 5});
        save | hide_cursor();
        // txbx | no_focus();
        load | strong_focus() | bg(Color::Green);

        // save | fixed_width(5);
        // save | minimum_width(2);
        // save | maximum_width(7);
        // save | preferred_width(7);
        // save | expanding_width(7);
        // save | minimum_expanding_width(7);
        // save | ignored_width();

        // save | width_hint(2);
        // save | width_min(2);
        // save | width_max(2);
        // save | width_stretch(2.3);
        // save | can_ignore_width_min();
        // save | cannot_ignore_width_min();

        // save | fixed_height(5);
        // save | minimum_height(2);
        // save | maximum_height(7);
        // save | preferred_height(7);
        // save | expanding_height(7);
        // save | minimum_expanding_height(7);
        // save | ignored_height();

        // save | height_hint(2);
        // save | height_min(2);
        // save | height_max(2);
        // save | height_stretch(2.3);
        // save | can_ignore_height_min();
        // save | cannot_ignore_height_min();

        // txbx | bordered() | block_walls_4() | west_wall(L'▓')
        //      | north_wall(L'▓') | north_west_corner(L'▓');

        // txbx | bordered() | block_walls_3() | north_west_walls(L'▓');
        txbx | bordered() | asterisk_walls() | plus_corners();
        load | on_mouse_press([&](auto const& m) {
            if (m.modifiers.ctrl)
                txbx | bg(Color::Orange);
        });
        // constexpr auto g = L'G' | Trait::Bold;
        auto gs = "hello" | Trait::Inverse;
    }
};

using namespace ox;

using Check      = Selectable<HCheckbox_label>;
using Check_list = Array<layout::Selecting<layout::Vertical<Check>>, 15>;

struct My_check_list : Check_list {
    My_check_list()
    {
        using namespace ox;
        using namespace ox::pipe;

        this->set_increment_selection_keys({Key::Arrow_down, Key::j});
        this->set_decrement_selection_keys({Key::Arrow_up, Key::k});
        this->set_increment_scroll_keys({Key::J});
        this->set_decrement_scroll_keys({Key::K});

        *this | /* passive_height() |*/ bordered() | children() |
            for_each([i = 0uL](auto& w) mutable {
                w.label.set_text("number: " + std::to_string(i++));
            });
        *this | bind_key(Key::Enter,
                         [](auto& w) { w.selected_child().checkbox.toggle(); });
    }
};

struct Two_lists : Pair<layout::Vertical<My_check_list>> {
    Two_lists()
    {
        *this | pipe::strong_focus() | pipe::on_focus_in([this] {
            ox::System::set_focus(this->get_children().front());
        });
        this->find_child_if(
            [](My_check_list const& l) { return l.name().empty(); });
        this->foo();
    }

    void foo() const
    {
        this->find_child_if(
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

        this->get<0>().set_label(L"I'm a BUTTON");
        this->get<0>() | on_press([this] { this->get<2>().toggle(); });

        this->get<1>().set_text(L"I'm a LABEL");
        this->get<2>().check();

        this->get<3>() | bg(Color::Dark_gray);

        // this->get<3>() | add_option(L"Option BLACK", [this] {
        //     this->get<0>() | bg(Color::Black);
        // });

        this->get<3>().add_option(L"Option BLACK").connect([this] {
            this->get<0>() | bg(Color::Black);
        });
        this->get<3>().add_option(L"Option RED").connect([this] {
            this->get<0>() | bg(Color::Red);
        });
        this->get<3>().add_option(L"Option GREEN").connect([this] {
            this->get<0>() | bg(Color::Green);
        });

        auto const s = std::to_string(sizeof(Glyph));
        this->get<4>().set_label(s);
        this->get<4>().set_pressed_color(Color::Violet);
        this->get<4>().set_released_color(Color::Blue);
    }
};

}  // namespace comp
#endif  // TERMOX_DEMOS_COMPOSITES_COMPOSITES_HPP
