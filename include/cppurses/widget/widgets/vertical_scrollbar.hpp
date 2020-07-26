#ifndef CPPURSES_WIDGET_WIDGETS_VERTICAL_SCROLLBAR_HPP
#define CPPURSES_WIDGET_WIDGETS_VERTICAL_SCROLLBAR_HPP
#include <signals/signal.hpp>
#include <signals/slot.hpp>

#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/button.hpp>

namespace cppurses {

class Vertical_scrollbar : public layout::Vertical<> {
   public:
    Button& up_button   = this->make_child<Button>("▴");
    Widget& middle      = this->make_child<Widget>();
    Button& down_button = this->make_child<Button>("▾");

    sig::Signal<void()>& up   = up_button.pressed;
    sig::Signal<void()>& down = down_button.pressed;

   public:
    Vertical_scrollbar() { this->initialize(); }

    template <typename Scrollable_t>
    explicit Vertical_scrollbar(Scrollable_t& tb)
    {
        this->initialize();
        auto scroll_up = sig::Slot<void()>{[&tb]() { tb.scroll_up(); }};
        scroll_up.track(tb.destroyed);
        up.connect(scroll_up);

        auto scroll_down = sig::Slot<void()>{[&tb]() { tb.scroll_down(); }};
        scroll_down.track(tb.destroyed);
        down.connect(scroll_down);
    }

   private:
    void initialize()
    {
        this->width_policy.fixed(1);
        this->height_policy.expanding(0);

        up_button.height_policy.fixed(1);

        middle.height_policy.expanding(0);
        middle.brush.set_background(Color::Light_gray);

        down_button.height_policy.fixed(1);
    }
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_VERTICAL_SCROLLBAR_HPP
