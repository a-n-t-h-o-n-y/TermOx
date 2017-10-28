#ifndef CPPURSES_WIDGET_WIDGETS_CYCLE_STACK_HPP
#define CPPURSES_WIDGET_WIDGETS_CYCLE_STACK_HPP
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/vertical_layout.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/widget_stack.hpp>

#include <memory>
#include <utility>

namespace cppurses {

class Cycle_stack : public Vertical_layout {
   public:
    Cycle_stack();

    template <typename T, typename... Args>
    T& make_page(Glyph_string title, Args&&... args);

    void add_page(Glyph_string title, std::unique_ptr<Widget> widget);

   private:
    struct Top_row;

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
