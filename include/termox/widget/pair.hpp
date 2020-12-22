#ifndef TERMOX_WIDGET_PAIR_HPP
#define TERMOX_WIDGET_PAIR_HPP

namespace ox {

/// Heterogeneous pair of Widgets within a Layout_t.
template <typename Layout_t,
          typename Widget_one_t = void,
          typename Widget_two_t = void>
class Pair : public Layout_t {
   public:
    Widget_one_t& first  = this->template make_child<Widget_one_t>();
    Widget_two_t& second = this->template make_child<Widget_two_t>();

   public:
    using Base = Layout_t;
};

template <typename Layout_t>
class Pair<Layout_t, void, void> : public Layout_t {
   public:
    typename Layout_t::Child_t& first  = this->template make_child();
    typename Layout_t::Child_t& second = this->template make_child();

   public:
    using Base = Layout_t;
};

}  // namespace ox
#endif  // TERMOX_WIDGET_PAIR_HPP
