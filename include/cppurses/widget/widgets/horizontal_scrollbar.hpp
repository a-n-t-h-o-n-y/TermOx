#ifndef CPPURSES_WIDGET_WIDGETS_HORIZONTAL_SCROLLBAR
#define CPPURSES_WIDGET_WIDGETS_HORIZONTAL_SCROLLBAR
#include <signals/signal.hpp>

#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/button.hpp>

namespace cppurses {

class Horizontal_scrollbar : public layout::Horizontal<> {
   public:
    Button& left_btn  = this->make_child<Button>("<");
    Widget& middle    = this->make_child<Widget>();
    Button& right_btn = this->make_child<Button>(">");

    sig::Signal<void()>& left  = left_btn.pressed;
    sig::Signal<void()>& right = right_btn.pressed;

   public:
    Horizontal_scrollbar()
    {
        using namespace pipe;
        *this | fixed_height(1) | expanding_width(0);

        left_btn | fixed_width(1);
        middle | expanding_width(0) | bg(Color::Light_gray);
        right_btn | fixed_width(1);
    }
};

/// Helper function to create an instance.
template <typename... Args>
auto horizontal_scrollbar(Args&&... args)
    -> std::unique_ptr<Horizontal_scrollbar>
{
    return std::make_unique<Horizontal_scrollbar>(std::forward<Args>(args)...);
}

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_HORIZONTAL_SCROLLBAR
