#ifndef CPPURSES_WIDGET_WIDGETS_HORIZONTAL_SCROLLBAR
#define CPPURSES_WIDGET_WIDGETS_HORIZONTAL_SCROLLBAR
#include <signals/signal.hpp>

#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/button.hpp>

namespace cppurses {

class Horizontal_scrollbar : public layout::Horizontal<> {
   public:
    Button& left_button  = this->make_child<Button>("<");
    Widget& middle       = this->make_child<Widget>();
    Button& right_button = this->make_child<Button>(">");

    sig::Signal<void()>& left  = left_button.clicked;
    sig::Signal<void()>& right = right_button.clicked;

   public:
    Horizontal_scrollbar()
    {
        this->height_policy.fixed(1);
        this->width_policy.expanding(0);

        left_button.width_policy.fixed(1);

        middle.width_policy.expanding(0);
        middle.brush.set_background(Color::Light_gray);

        right_button.width_policy.fixed(1);
    }
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_HORIZONTAL_SCROLLBAR
