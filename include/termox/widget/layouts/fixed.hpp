#ifndef TERMOX_WIDGET_LAYOUT_FIXED_HPP
#define TERMOX_WIDGET_LAYOUT_FIXED_HPP
#include <cassert>

#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

/// Fixed offset before and after Widget_t object.
template <template <typename> typename Layout_t, typename Widget_t>
class Fixed : public Layout_t<Widget> {
   private:
    using Base_t = Layout_t<Widget>;

    static auto constexpr is_horizontal = layout::is_horizontal_v<Base_t>;

   public:
    Widget& buffer_1;
    Widget_t& widget;
    Widget& buffer_2;

   public:
    template <typename... Args>
    Fixed(int offset_1, int offset_2, Args&&... args)
        : buffer_1{this->template make_child()},
          widget{
              this->template make_child<Widget_t>(std::forward<Args>(args)...)},
          buffer_2{this->template make_child()},
          offset_1_{offset_1},
          offset_2_{offset_2}
    {
        assert(offset_1_ >= 0 && offset_2_ >= 0);
        this->update_policies(offset_1_, offset_2_);
        if constexpr (is_horizontal) {
            buffer_1 | pipe::fixed_width(offset_1_);
            buffer_2 | pipe::fixed_width(offset_2_);
        }
        else {
            buffer_1 | pipe::fixed_height(offset_1_);
            buffer_2 | pipe::fixed_height(offset_2_);
        }
    }

    template <typename... Args>
    Fixed(int offset, Args&&... args)
        : Fixed{offset, offset, std::forward<Args>(args)...}
    {}

   protected:
    auto child_polished_event(Widget& child) -> bool override
    {
        if (&child == &widget)
            this->update_policies(offset_1_, offset_2_);
        return Base_t::child_polished_event(child);
    }

   private:
    int offset_1_;
    int offset_2_;

   private:
    void update_policies(int offset_1, int offset_2)
    {
        auto const offset_policy = [=](Size_policy& sp) {
            auto const offset_total = offset_1 + offset_2;
            sp.hint(sp.hint() + offset_total);
            sp.min(sp.min() + offset_total);
            if (sp.max() != Size_policy::maximum_max)
                sp.max(sp.max() + offset_total);
        };

        this->height_policy = widget.height_policy;
        this->width_policy  = widget.width_policy;

        if constexpr (is_horizontal)
            offset_policy(this->width_policy);
        else
            offset_policy(this->width_policy);
    }
};

template <typename Widget_t>
using HFixed = Fixed<layout::Horizontal, Widget_t>;

template <typename Widget_t, typename... Args>
[[nodiscard]] inline auto hfixed(Args&&... args)
    -> std::unique_ptr<HFixed<Widget_t>>
{
    return std::make_unique<HFixed<Widget_t>>(std::forward<Args>(args)...);
}

template <typename Widget_t>
using VFixed = Fixed<layout::Vertical, Widget_t>;

template <typename Widget_t, typename... Args>
[[nodiscard]] inline auto vfixed(Args&&... args)
    -> std::unique_ptr<VFixed<Widget_t>>
{
    return std::make_unique<VFixed<Widget_t>>(std::forward<Args>(args)...);
}

template <typename Widget_t>
class Fixed_2d : public HFixed<VFixed<Widget_t>> {
    using Base_t = HFixed<VFixed<Widget_t>>;

   public:
    template <typename... Args>
    Fixed_2d(int north, int south, int east, int west, Args&&... args)
        : Base_t{west, east, north, south, std::forward<Args>(args)...}
    {}

   public:
    Widget_t& widget = this->Base_t::widget.widget;
};

template <typename Widget_t, typename... Args>
[[nodiscard]] inline auto fixed_2d(Args&&... args)
    -> std::unique_ptr<Fixed_2d<Widget_t>>
{
    return std::make_unique<Fixed_2d<Widget_t>>(std::forward<Args>(args)...);
}

}  // namespace ox
#endif  // TERMOX_WIDGET_LAYOUT_FIXED_HPP
