#ifndef CPPURSES_WIDGET_WIDGETS_CYCLE_STACK_HPP
#define CPPURSES_WIDGET_WIDGETS_CYCLE_STACK_HPP
#include <memory>
#include <utility>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/cycle_box.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace cppurses {
class Widget;

/// A layout::Stack with an interface to cycle through each Widget in the stack.
class Cycle_stack : public layout::Vertical {
   public:
    /// Construct a new T object and add it to the end of the Stack.
    /** Returns a reference to this newly created page. \p title is passed to
     *  the Cycle_box to display when this page is active. */
    template <typename T, typename... Args>
    T& make_page(Glyph_string title, Args&&... args);

    /// Append a page to the Stack.
    /** \p title is passed to the Cycle_box associated with this page. */
    void append_page(Glyph_string title, std::unique_ptr<Widget> widget);

    /// User interface to cycle through the pages of the Stack.
    struct Top_row : public layout::Horizontal {
        Top_row();
        Push_button& left_btn{this->make_child<Push_button>("⏴")};
        Cycle_box& cycle_box{this->make_child<Cycle_box>()};
        Push_button& right_btn{this->make_child<Push_button>("⏵")};
    };

    Top_row& top_row{this->make_child<Top_row>()};
    layout::Stack& stack{this->make_child<layout::Stack>()};
};

// - - - - - - - - - - - - Template Implementations - - - - - - - - - - - - - -

template <typename T, typename... Args>
T& Cycle_stack::make_page(Glyph_string title, Args&&... args) {
    std::unique_ptr<T> u_ptr = std::make_unique<T>(std::forward<Args>(args)...);
    T& ref{*u_ptr};
    this->append_page(std::move(title), std::move(u_ptr));
    return ref;
}

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_CYCLE_STACK_HPP
