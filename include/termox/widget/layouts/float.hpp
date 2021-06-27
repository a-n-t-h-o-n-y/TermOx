#ifndef TERMOX_WIDGET_LAYOUT_FLOAT_HPP
#define TERMOX_WIDGET_LAYOUT_FLOAT_HPP
#include <memory>
#include <utility>

#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

// TODO limit child add/remove functions, invariant should be only every having
// the widgets you have at construction.

/// Holds a given \p Widget_t object between two empty 'buffer' Widgets.
/** The two 'buffer' Widgets have default constructed size policies. */
template <template <typename> typename Layout_t, typename Widget_t>
class Float : public Layout_t<Widget> {
   private:
    using Base_t = Layout_t<Widget>;

   public:
    Widget& buffer_1;
    Widget_t& widget;
    Widget& buffer_2;

   public:
    template <typename... Args>
    Float(Args&&... args)
        : buffer_1{this->template make_child()},
          widget{
              this->template make_child<Widget_t>(std::forward<Args>(args)...)},
          buffer_2{this->template make_child()}
    {
        this->update_policy();
    }

   protected:
    auto child_polished_event(Widget& child) -> bool override
    {
        if (&child == &widget)
            this->update_policy();
        return Base_t::child_polished_event(child);
    }

   private:
    void update_policy()
    {
        if constexpr (layout::is_horizontal_v<Base_t>)
            this->height_policy = widget.height_policy;
        else
            this->width_policy = widget.width_policy;
    }
};

template <typename Widget_t>
using VFloat = Float<layout::Vertical, Widget_t>;

template <typename Widget_t, typename... Args>
[[nodiscard]] inline auto vfloat(Args&&... args)
    -> std::unique_ptr<VFloat<Widget_t>>
{
    return std::make_unique<VFloat<Widget_t>>(std::forward<Args>(args)...);
}

template <typename Widget_t>
using HFloat = Float<layout::Horizontal, Widget_t>;

template <typename Widget_t, typename... Args>
[[nodiscard]] inline auto hfloat(Args&&... args)
    -> std::unique_ptr<HFloat<Widget_t>>
{
    return std::make_unique<HFloat<Widget_t>>(std::forward<Args>(args)...);
}

template <typename Widget_t>
using Float_2d = VFloat<HFloat<Widget_t>>;

template <typename Widget_t, typename... Args>
[[nodiscard]] inline auto float_2d(Args&&... args)
    -> std::unique_ptr<Float_2d<Widget_t>>
{
    return std::make_unique<Float_2d<Widget_t>>(std::forward<Args>(args)...);
}

}  // namespace ox
#endif  //
