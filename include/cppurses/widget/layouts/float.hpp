#ifndef CPPURSES_WIDGET_LAYOUT_FLOAT_HPP
#define CPPURSES_WIDGET_LAYOUT_FLOAT_HPP
#include <utility>

#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

// TODO make it a template template arg for Layout_t
// the wrapped widget should never be able to be changed, same for the buffers,
// so you need to hide the underlying Layout_t modification methods from others
// who might use this class.
template <typename Widget_t, template <typename> typename Layout_t>
class Float : public Layout_t<Widget> {
   public:
    cppurses::Widget& buffer_1;
    Widget_t& widget;
    cppurses::Widget& buffer_2;

   public:
    // TODO should take two stretch values... but should it mix that in with the
    // constructor args? they can't be defaulted in this way, instead maybe have
    // two member functions that set these stretch values.

    // The problem is that you don't track changes to the widgets size, so
    // connect to the widget's size policy signal and update it when it changes.
    template <typename... Args>
    Float(Args&&... args)
        : buffer_1{this->make_child()},
          widget{
              this->template make_child<Widget_t>(std::forward<Args>(args)...)},
          buffer_2{this->make_child()}
    {
        if constexpr (cppurses::layout::is_horizontal_v<Layout_t<Widget>>) {
            this->height_policy = widget.height_policy;
            widget.height_policy.policy_updated.connect(
                [this] { this->height_policy = widget.height_policy; });
        }
        else {
            this->width_policy = widget.width_policy;
            widget.width_policy.policy_updated.connect(
                [this] { this->width_policy = widget.width_policy; });
        }
    }
};

}  // namespace cppurses
#endif  //
