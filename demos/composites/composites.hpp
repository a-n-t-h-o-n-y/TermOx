#ifndef CPPURSES_DEMOS_COMPOSITES_COMPOSITES_HPP
#define CPPURSES_DEMOS_COMPOSITES_COMPOSITES_HPP
#include <utility>

#include <cppurses/widget/array.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/tuple.hpp>
#include <cppurses/widget/widgets/push_button.hpp>
#include <cppurses/widget/widgets/textbox.hpp>

namespace comp {

// TODO namespace for these policy(?) types?

template <typename Widget_t>
struct Border : Widget_t {
    Border() { this->border.enable(); }
};

template <typename Widget_t>
struct Border_east : Widget_t {
    Border_east()
    {
        this->border.enable();
        this->border.segments.disable_all();
        this->border.segments.east.enable();
    }
};

template <typename Widget_t>
struct Border_west : Widget_t {
    Border_west()
    {
        this->border.enable();
        this->border.segments.disable_all();
        this->border.segments.west.enable();
    }
};

template <typename Widget_t>
struct Border_north : Widget_t {
    Border_north()
    {
        this->border.enable();
        this->border.segments.disable_all();
        this->border.segments.north.enable();
    }
};

template <typename Widget_t>
struct Border_south : Widget_t {
    Border_south()
    {
        this->border.enable();
        this->border.segments.disable_all();
        this->border.segments.south.enable();
    }
};

template <typename Widget_t, cppurses::Color Bg>
struct Background : Widget_t {
    Background() { this->brush.set_background(Bg); }
};

using Foo = cppurses::Tuple<
    cppurses::layout::Horizontal<>,
    cppurses::Push_button,
    Background<cppurses::Push_button, cppurses::Color::Orange>,
    Background<Border_east<cppurses::Textbox>, cppurses::Color::Violet>,
    cppurses::Push_button>;

// struct Composites : Foo {
//     Composites()
//     {
//         auto& btn_1 = this->get<0>();
//         auto& btn_2 = this->get<1>();
//         auto& txbx  = this->get<2>();
//         auto& btn_3 = this->get<3>();

//         using namespace cppurses;
//         btn_1.brush.set_background(Color::Light_blue);
//         btn_1.set_label(L"X");
//         btn_2.set_label(L"Y");
//         txbx.set_contents("HeLLO");
//         btn_3.set_label(L"Z");
//     }
// };

// TODO
// SFINAE so it doesn't ... make member of Widget?
// but then you can't return a more derived type.
template <typename Widget_t, typename Lambda>
auto operator|(Widget_t& w, Lambda op) -> Widget_t&
{
    op(w);
    return w;
}

// TODO need namespace for all of this
auto background(cppurses::Color c)
{
    return [c](auto& w) { w.brush.set_background(c); };
}

auto foreground(cppurses::Color c)
{
    return [c](auto& w) { w.brush.set_foreground(c); };
}

auto label(cppurses::Glyph_string l)
{
    return [label = std::move(l)](auto& w) { w.set_label(std::move(label)); };
}

auto border()
{
    return [](auto& w) { w.border.enable(); };
}

auto fixed_width(std::size_t amount)
{
    return [amount](auto& w) { w.width_policy.fixed(amount); };
}

auto fixed_height(std::size_t amount)
{
    return [amount](auto& w) { w.height_policy.fixed(amount); };
}

// struct Btn : cppurses::layout::Horizontal<> {
//     cppurses::Push_button& btn =
//         this->make_child<cppurses::Push_button>() | label("btn") |
//         comp::background(cppurses::Color::Red) | comp::border();
// };

using Btns =
    cppurses::Array<cppurses::layout::Horizontal<>, cppurses::Push_button, 2>;

using App =
    cppurses::Tuple<cppurses::layout::Vertical<>, cppurses::Textbox, Btns>;

struct Composites : App {
    Composites()
    {
        auto& btns = this->get<1>();
        auto& save = btns.get<0>();
        auto& load = btns.get<1>();
        auto& txbx = this->get<0>();

        btns | fixed_height(1);
        save | label("Save") | comp::background(cppurses::Color::Light_blue);
        load | label("Load") | comp::background(cppurses::Color::Light_green);
        txbx | comp::background(cppurses::Color::White) |
            comp::foreground(cppurses::Color::Black);
    }
};

}  // namespace comp
#endif  // CPPURSES_DEMOS_COMPOSITES_COMPOSITES_HPP
