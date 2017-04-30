#ifndef HORIZONTAL_SCROLLBAR
#define HORIZONTAL_SCROLLBAR

#include "../layouts/horizontal_layout.hpp"
#include "push_button.hpp"

namespace twf {

class Horizontal_scrollbar : public Horizontal_layout {
   public:
    Push_button& left_button = this->make_child<Push_button>("⯇");
    Widget& middle = this->make_child<Widget>();
    Push_button& right_button = this->make_child<Push_button>("⯈");
};

} // namespace twf;

#endif // HORIZONTAL_SCROLLBAR
