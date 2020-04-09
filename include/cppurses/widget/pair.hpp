#ifndef CPPURSES_WIDGET_PAIR_HPP
#define CPPURSES_WIDGET_PAIR_HPP

namespace cppurses {

/// Heterogeneous pair of Widgets within a Layout_t.
template <typename Layout_t, typename Widget_one_t, typename Widget_two_t>
class Pair : public Layout_t {
   public:
    Widget_one_t& first  = this->template make_child<Widget_one_t>();
    Widget_two_t& second = this->template make_child<Widget_two_t>();
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_PAIR_HPP
