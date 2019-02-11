#ifndef CPPURSES_WIDGET_WIDGETS_CYCLE_STACK_HPP
#define CPPURSES_WIDGET_WIDGETS_CYCLE_STACK_HPP
#include <memory>
#include <string>
#include <utility>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/cycle_box.hpp>
#include <cppurses/widget/widgets/push_button.hpp>
#include <cppurses/widget/widgets/widget_stack.hpp>

namespace cppurses {
class Widget;

/// A Widget_stack with an interface to cycle through each Widget in the stack.
class Cycle_stack : public layout::Vertical {
   public:
    Cycle_stack();

    template <typename T, typename... Args>
    T& make_page(Glyph_string title, Args&&... args);

    void add_page(Glyph_string title, std::unique_ptr<Widget> widget);

    struct Top_row : public layout::Horizontal {
        Top_row();
        Push_button& left_btn{this->make_child<Push_button>("⏴")};
        Cycle_box& cycle_box{this->make_child<Cycle_box>()};
        Push_button& right_btn{this->make_child<Push_button>("⏵")};
    };

    Top_row& top_row;
    Widget_stack& stack{this->make_child<Widget_stack>()};
};

// - - - - - - - - - - - - Template Implementations - - - - - - - - - - - - - -

template <typename T, typename... Args>
T& Cycle_stack::make_page(Glyph_string title, Args&&... args) {
    std::unique_ptr<T> u_ptr = std::make_unique<T>(std::forward<Args>(args)...);
    T& ref{*u_ptr};
    this->add_page(std::move(title), std::move(u_ptr));
    return ref;
}

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_CYCLE_STACK_HPP
