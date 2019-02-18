#ifndef CPPURSES_WIDGET_WIDGETS_HORIZONTAL_SCROLLBAR
#define CPPURSES_WIDGET_WIDGETS_HORIZONTAL_SCROLLBAR
#include <signals/signal.hpp>

#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace cppurses {

class Horizontal_scrollbar : public layout::Horizontal {
   public:
    Horizontal_scrollbar();

    Push_button& left_button = this->make_child<Push_button>("◂");
    Widget& middle = this->make_child<Widget>();
    Push_button& right_button = this->make_child<Push_button>("▸");

    // Signals
    sig::Signal<void()>& left = left_button.clicked;
    sig::Signal<void()>& right = right_button.clicked;
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_HORIZONTAL_SCROLLBAR
