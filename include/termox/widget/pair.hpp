#ifndef TERMOX_WIDGET_PAIR_HPP
#define TERMOX_WIDGET_PAIR_HPP
#include <memory>

#include <termox/common/common_type.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/stack.hpp>
#include <termox/widget/layouts/vertical.hpp>

namespace ox {

/// Heterogeneous pair of Widgets within a Layout_t.
template <typename Layout_t,
          typename Widget_one_t = void,
          typename Widget_two_t = void>
class Pair : public Layout_t {
   public:
    using Base = Layout_t;

   public:
    Widget_one_t& first;
    Widget_two_t& second;

   public:
    // TODO if both types have Parameters defines, then take those, default to
    // default constructed?
    Pair()
        : first{this->template make_child<Widget_one_t>()},
          second{this->template make_child<Widget_two_t>()}
    {}

    Pair(std::unique_ptr<Widget_one_t> a, std::unique_ptr<Widget_two_t> b)
        : Base{std::move(a), std::move(b)},
          first{static_cast<Widget_one_t&>(this->get_children()[0])},
          second{static_cast<Widget_two_t&>(this->get_children()[1])}
    {}
};

template <typename Layout_t>
class Pair<Layout_t, void, void> : public Layout_t {
   public:
    typename Layout_t::Child_t& first  = this->template make_child();
    typename Layout_t::Child_t& second = this->template make_child();

   public:
    using Base = Layout_t;
};

// TODO add constructor params.
template <typename Layout_t,
          typename Widget_one_t = void,
          typename Widget_two_t = void>
auto pair() -> std::unique_ptr<Pair<Layout_t, Widget_one_t, Widget_two_t>>
{
    return std::make_unique<Pair<Layout_t, Widget_one_t, Widget_two_t>>();
}

// TODO make function() -> unique_ptr for each
template <typename First, typename Second>
using VPair = Pair<layout::Vertical<Common_type<First, Second>>, First, Second>;

template <typename First, typename Second>
using HPair =
    Pair<layout::Horizontal<Common_type<First, Second>>, First, Second>;

template <typename First, typename Second>
using SPair = Pair<layout::Stack<Common_type<First, Second>>, First, Second>;

}  // namespace ox
#endif  // TERMOX_WIDGET_PAIR_HPP
