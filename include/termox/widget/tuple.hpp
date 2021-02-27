#ifndef TERMOX_WIDGET_TUPLE_HPP
#define TERMOX_WIDGET_TUPLE_HPP
#include <memory>
#include <tuple>
#include <utility>

#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>

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
        : refs_{this->indexed_init_children(
              std::index_sequence_for<Widget_t...>(),
              std::move(p)...)}
    {}

   public:
    /// Get child by index.
    template <std::size_t Index>
    [[nodiscard]] auto get() -> auto&
    {
        return std::get<Index>(refs_);
    }

    /// Get child by index.
    template <std::size_t Index>
    [[nodiscard]] auto get() const -> auto const&
    {
        return std::get<Index>(refs_);
    }

   private:
    using Refs_tuple = std::tuple<Widget_t&...>;
    Refs_tuple refs_ =
        this->indexed_init_children(std::index_sequence_for<Widget_t...>());

   private:
    template <std::size_t... Is>
    auto indexed_init_children(std::index_sequence<Is...>,
                               typename Widget_t::Parameters... p)
    {
        return std::forward_as_tuple(this->insert_child(
            std::make_unique<Widget_t>(std::move(p)), Is)...);
    }

    template <std::size_t... Is>
    auto indexed_init_children(std::index_sequence<Is...>)
    {
        return std::forward_as_tuple(
            this->insert_child(std::make_unique<Widget_t>(), Is)...);
    }
};

template <typename Layout_t, typename... Widget_t>
auto tuple(typename Widget_t::Parameters... p)
    -> std::unique_ptr<Tuple<Layout_t, Widget_t...>>
{
    return std::make_unique<Tuple<Layout_t, Widget_t...>>(std::move(p)...);
}

template <typename Layout_t, typename... Widget_t>
auto tuple() -> std::unique_ptr<Tuple<Layout_t, Widget_t...>>
{
    return std::make_unique<Tuple<Layout_t, Widget_t...>>();
}

// TODO make function() -> unique_ptr for each
template <typename... Widget_t>
using VTuple = Tuple<layout::Vertical<>, Widget_t...>;

template <typename... Widget_t>
using HTuple = Tuple<layout::Horizontal<>, Widget_t...>;

}  // namespace ox
#endif  // TERMOX_WIDGET_TUPLE_HPP
