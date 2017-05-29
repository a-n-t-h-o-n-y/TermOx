#ifndef WIDGET_MODULE_WIDGETS_VERTICAL_SCROLLBAR_HPP
#define WIDGET_MODULE_WIDGETS_VERTICAL_SCROLLBAR_HPP

#include "widget_module/layouts/vertical_layout.hpp"
#include "widget_module/widget.hpp"
#include "widget_module/widgets/push_button.hpp"
#include <aml/signals/signals.hpp>

namespace cppurses {

class Vertical_scrollbar : public Vertical_layout {
   public:
    Vertical_scrollbar();

    Push_button& up_button = this->make_child<Push_button>("▴");
    Widget& middle = this->make_child<Widget>();
    Push_button& down_button = this->make_child<Push_button>("▾");

    // Signals
    sig::Signal<void()>& up = up_button.clicked;
    sig::Signal<void()>& down = down_button.clicked;
};

}  // namespace cppurses
#endif  // WIDGET_MODULE_WIDGETS_VERTICAL_SCROLLBAR_HPP
