#ifndef CPPURSES_WIDGET_WIDGETS_CYCLE_STACK_HPP
#define CPPURSES_WIDGET_WIDGETS_CYCLE_STACK_HPP
#include <memory>
#include <type_traits>
#include <utility>

#include <signals/signal.hpp>

#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/trait.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/button.hpp>
#include <cppurses/widget/widgets/cycle_box.hpp>

namespace cppurses {

/// A layout::Stack with an interface to cycle through each Widget in the stack.
template <typename Child = Widget>
class Cycle_stack : public layout::Vertical<Widget> {
   private:
    /// User interface to cycle through the pages of the Stack.
    class Top_row : public layout::Horizontal<Widget> {
       public:
        Button& left_btn     = this->make_child<Button>("<");
        Cycle_box& cycle_box = this->make_child<Cycle_box>();
        Button& right_btn    = this->make_child<Button>(">");

       public:
        Top_row()
        {
            using namespace pipe;
            *this | fixed_height(1) | children() | bg(Color::Light_gray) |
                fg(Color::Black);

            left_btn | fixed_width(1) | on_press(slot::previous(cycle_box));
            right_btn | fixed_width(1) | on_press(slot::next(cycle_box));
            cycle_box | add(Trait::Bold);
        }
    };

   public:
    Top_row& top_row            = this->make_child<Top_row>();
    layout::Stack<Child>& stack = this->make_child<layout::Stack<Child>>();

   public:
    /// Construct a new Widget_t object and add it to the end of the Stack.
    /** Returns a reference to this newly created page. \p title is passed to
     *  the Cycle_box to display when this page is active. */
    template <typename Widget_t = Child, typename... Args>
    auto make_page(Glyph_string title, Args&&... args) -> Widget_t&
    {
        static_assert(std::is_base_of<Child, Widget_t>::value,
                      "Cycle_stack::make_page: Widget_t must be a Child type");
        auto child = std::make_unique<Widget_t>(std::forward<Args>(args)...);
        return static_cast<Widget_t&>(
            this->append_page(std::move(title), std::move(child)));
    }

    /// Append a page to the Stack.
    /** \p title is passed to the Cycle_box associated with this page. */
    auto append_page(Glyph_string title, std::unique_ptr<Child> widget)
        -> Child&
    {
        auto& signal = top_row.cycle_box.add_option(std::move(title));
        signal.connect(slot::set_active_page(stack, stack.size()));
        auto& child = stack.append_page(std::move(widget));
        if (stack.size() == 1)
            stack.set_active_page(0);
        return child;
    }
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_CYCLE_STACK_HPP
