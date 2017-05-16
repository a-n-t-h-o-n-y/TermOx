#ifndef WIDGET_MODULE_WIDGETS_HORIZONTAL_SCROLLBAR
#define WIDGET_MODULE_WIDGETS_HORIZONTAL_SCROLLBAR

#include <aml/signals/signals.hpp>
#include "widget_module/layouts/horizontal_layout.hpp"
#include "widget_module/widget.hpp"
#include "widget_module/widgets/push_button.hpp"

namespace twf {

class Horizontal_scrollbar : public Horizontal_layout {
   public:
    Horizontal_scrollbar();

    Push_button& left_button = this->make_child<Push_button>("◂");
    Widget& middle = this->make_child<Widget>();
    Push_button& right_button = this->make_child<Push_button>("▸");

    // Signals
    sig::Signal<void()>& left = left_button.clicked;
    sig::Signal<void()>& right = right_button.clicked;
};

}  // namespace twf;

#endif  // WIDGET_MODULE_WIDGETS_HORIZONTAL_SCROLLBAR
