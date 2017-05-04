#ifndef VERTICAL_SCROLLBAR_HPP
#define VERTICAL_SCROLLBAR_HPP

#include <aml/signals/signals.hpp>

#include "../layouts/vertical_layout.hpp"
#include "push_button.hpp"

namespace twf {

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

}  // namespace twf
#endif  // VERTICAL_SCROLLBAR_HPP
