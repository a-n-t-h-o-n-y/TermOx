#ifndef CPPURSES_WIDGET_WIDGETS_VERTICAL_SCROLLBAR_HPP
#define CPPURSES_WIDGET_WIDGETS_VERTICAL_SCROLLBAR_HPP
#include <signals/signal.hpp>

#include <cppurses/widget/layouts/vertical_layout.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/push_button.hpp>
#include <cppurses/widget/widgets/textbox_base.hpp>

namespace cppurses {

class Vertical_scrollbar : public Vertical_layout {
   public:
    Vertical_scrollbar();
    Vertical_scrollbar(Textbox_base& tb);

    Push_button& up_button = this->make_child<Push_button>("▴");
    Widget& middle = this->make_child<Widget>();
    Push_button& down_button = this->make_child<Push_button>("▾");

    // Signals
    sig::Signal<void()>& up = up_button.clicked;
    sig::Signal<void()>& down = down_button.clicked;

   private:
    void initialize();
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_VERTICAL_SCROLLBAR_HPP
