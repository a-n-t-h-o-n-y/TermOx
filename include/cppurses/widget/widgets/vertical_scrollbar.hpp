#ifndef CPPURSES_WIDGET_WIDGETS_VERTICAL_SCROLLBAR_HPP
#define CPPURSES_WIDGET_WIDGETS_VERTICAL_SCROLLBAR_HPP
#include <signals/signal.hpp>
#include <signals/slot.hpp>

#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace cppurses {

class Vertical_scrollbar : public layout::Vertical {
   public:
    Vertical_scrollbar();

    template <typename Scrollable_t>
    Vertical_scrollbar(Scrollable_t& tb) {
        this->initialize();
        sig::Slot<void()> scroll_up{[&tb]() { tb.scroll_up(); }};
        scroll_up.track(tb.destroyed);
        up.connect(scroll_up);

        sig::Slot<void()> scroll_down{[&tb]() { tb.scroll_down(); }};
        scroll_down.track(tb.destroyed);
        down.connect(scroll_down);
    }

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
