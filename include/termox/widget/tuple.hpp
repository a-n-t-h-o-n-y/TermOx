#ifndef TERMOX_WIDGET_TUPLE_HPP
#define TERMOX_WIDGET_TUPLE_HPP
#include <tuple>
#include <utility>

namespace ox {

/// Heterogeneous collection of Widgets within a Layout_t.
/** Widget_t's must be default constructible. Widgets are constructed in the
 *  order that types are passed in. */
template <typename Layout_t, typename... Widget_t>
class Tuple : public Layout_t {
   public:
    using Base = Layout_t;

   public:
    Tuple() = default;

    explicit Tuple(typename Widget_t::Parameters... p)
        : refs_{std::forward_as_tuple(
              this->template make_child<Widget_t>(std::move(p))...)}
    {}

   public:
    /// Get child by index.
    template <std::size_t Index>
    auto get() -> auto&
    {
        return std::get<Index>(refs_);
    }

    /// Get child by index.
    template <std::size_t Index>
    auto get() const -> auto const&
    {
        return std::get<Index>(refs_);
    }

   private:
    std::tuple<Widget_t&...> refs_ =
        std::forward_as_tuple(this->template make_child<Widget_t>()...);
};

}  // namespace ox
#endif  // TERMOX_WIDGET_TUPLE_HPP
