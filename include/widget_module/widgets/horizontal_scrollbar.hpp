#ifndef HORIZONTAL_SCROLLBAR
#define HORIZONTAL_SCROLLBAR

#include <aml/signals/signals.hpp>

#include "../layouts/horizontal_layout.hpp"
#include "push_button.hpp"

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

#endif  // HORIZONTAL_SCROLLBAR
