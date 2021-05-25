#ifndef TERMOX_WIDGET_TUPLE_HPP
#define TERMOX_WIDGET_TUPLE_HPP
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/stack.hpp>
#include <termox/widget/layouts/vertical.hpp>

namespace ox {

/// Heterogeneous collection of Widgets within a Layout_t.
/** Widgets are added to Layout_t in the order that Widget_t types are given. */
template <typename Layout_t, typename... Widget_t>
class Tuple : public Layout_t {
   public:
    using Parameters = std::tuple<typename Widget_t::Parameters...>;

   public:
    /// Default construct each Widget.
    Tuple()
        : refs_{this->indexed_init_children(
              std::index_sequence_for<Widget_t...>())}
    {}

    /// Construct each Widget with a Widget_t::Parameters object.
    explicit Tuple(typename Widget_t::Parameters... p)
        : refs_{this->indexed_init_children(
              std::index_sequence_for<Widget_t...>(),
              std::move(p)...)}
    {}

    /// Construct from a Tuple::Parameters object.
    explicit Tuple(Tuple::Parameters p)
        : refs_{this->tuple_init_children(
              std::make_index_sequence<std::tuple_size_v<Tuple::Parameters>>(),
              std::move(p))}
    {}

    /// Move existing Widgets into a Tuple.
    explicit Tuple(std::unique_ptr<Widget_t>... widget_ptrs)
        : refs_{this->indexed_move_children(
              std::index_sequence_for<Widget_t...>(),
              std::move(widget_ptrs)...)}
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
    std::tuple<Widget_t&...> refs_;

   private:
    template <std::size_t... Is>
    [[nodiscard]] auto indexed_init_children(std::index_sequence<Is...>)
    {
        return std::forward_as_tuple(
            this->insert_child(std::make_unique<Widget_t>(), Is)...);
    }

    template <std::size_t... Is>
    [[nodiscard]] auto indexed_init_children(std::index_sequence<Is...>,
                                             typename Widget_t::Parameters... p)
    {
        // Default construct Widget_t if it doesn't have Parameters constructor.
        // All widgets have Parameters struct inherited from Widget.
        return std::forward_as_tuple(this->insert_child(
            [&] {
                if constexpr (std::is_constructible_v<
                                  Widget_t, typename Widget_t::Parameters>)
                    return std::make_unique<Widget_t>(std::move(p));
                else
                    return std::make_unique<Widget_t>();
            }(),
            Is)...);
    }

    template <std::size_t... Is>
    [[nodiscard]] auto indexed_move_children(
        std::index_sequence<Is...>,
        std::unique_ptr<Widget_t>... widget_ptrs)
    {
        return std::forward_as_tuple(
            this->insert_child(std::move(widget_ptrs), Is)...);
    }

    template <std::size_t... Is>
    [[nodiscard]] auto tuple_init_children(std::index_sequence<Is...>,
                                           Tuple::Parameters parameters)
    {
        return std::forward_as_tuple(this->insert_child(
            std::make_unique<Widget_t>(std::move(std::get<Is>(parameters))),
            Is)...);
    }

   private:
    using Layout_t::append_child;
    using Layout_t::delete_all_children;
    using Layout_t::insert_child;
    using Layout_t::make_child;
    using Layout_t::remove_and_delete_child;
    using Layout_t::remove_and_delete_child_at;
    using Layout_t::remove_and_delete_child_if;
    using Layout_t::remove_child;
    using Layout_t::remove_child_at;
    using Layout_t::remove_child_if;
    using Layout_t::swap_children;
};

/// Build a default constructed Tuple unique_ptr.
template <typename Layout_t, typename... Widget_t>
[[nodiscard]] auto tuple() -> std::unique_ptr<Tuple<Layout_t, Widget_t...>>
{
    return std::make_unique<Tuple<Layout_t, Widget_t...>>();
}

/// Build a Tuple unique_ptr from Widget_t::Parameter objects.
template <typename Layout_t, typename... Widget_t>
[[nodiscard]] auto tuple(typename Widget_t::Parameters... p)
    -> std::unique_ptr<Tuple<Layout_t, Widget_t...>>
{
    return std::make_unique<Tuple<Layout_t, Widget_t...>>(std::move(p)...);
}

/// Build a Tuple from existing Widgets.
template <typename Layout_t, typename... Widget_t>
[[nodiscard]] auto tuple(std::unique_ptr<Widget_t>... widget_ptrs)
    -> std::unique_ptr<Tuple<Layout_t, Widget_t...>>
{
    return std::make_unique<Tuple<Layout_t, Widget_t...>>(
        std::move(widget_ptrs)...);
}

/// A Vertical Linear Layout Tuple.
template <typename... Widget_t>
using VTuple = Tuple<layout::Vertical<>, Widget_t...>;

/// Build a default constructed VTuple unique_ptr.
template <typename... Widget_t>
[[nodiscard]] auto vtuple() -> std::unique_ptr<VTuple<Widget_t...>>
{
    return std::make_unique<VTuple<Widget_t...>>();
}

/// Build a VTuple unique_ptr from Widget_t::Parameter objects.
template <typename... Widget_t>
[[nodiscard]] auto vtuple(typename Widget_t::Parameters... p)
    -> std::unique_ptr<VTuple<Widget_t...>>
{
    return std::make_unique<VTuple<Widget_t...>>(std::move(p)...);
}

/// Build a VTuple from existing Widgets.
template <typename... Widget_t>
[[nodiscard]] auto vtuple(std::unique_ptr<Widget_t>... widget_ptrs)
    -> std::unique_ptr<VTuple<Widget_t...>>
{
    return std::make_unique<VTuple<Widget_t...>>(std::move(widget_ptrs)...);
}

/// A Horizontal Linear Layout Tuple.
template <typename... Widget_t>
using HTuple = Tuple<layout::Horizontal<>, Widget_t...>;

/// Build a default constructed HTuple unique_ptr.
template <typename... Widget_t>
[[nodiscard]] auto htuple() -> std::unique_ptr<HTuple<Widget_t...>>
{
    return std::make_unique<HTuple<Widget_t...>>();
}

/// Build an HTuple unique_ptr from Widget_t::Parameter objects.
template <typename... Widget_t>
[[nodiscard]] auto htuple(typename Widget_t::Parameters... p)
    -> std::unique_ptr<HTuple<Widget_t...>>
{
    return std::make_unique<HTuple<Widget_t...>>(std::move(p)...);
}

/// Build an HTuple from existing Widgets.
template <typename... Widget_t>
[[nodiscard]] auto htuple(std::unique_ptr<Widget_t>... widget_ptrs)
    -> std::unique_ptr<HTuple<Widget_t...>>
{
    return std::make_unique<HTuple<Widget_t...>>(std::move(widget_ptrs)...);
}

/// A Stack Layout Tuple.
template <typename... Widget_t>
using STuple = Tuple<layout::Stack<>, Widget_t...>;

/// Build a default constructed STuple unique_ptr.
template <typename... Widget_t>
[[nodiscard]] auto stuple() -> std::unique_ptr<STuple<Widget_t...>>
{
    return std::make_unique<STuple<Widget_t...>>();
}

/// Build an STuple unique_ptr from Widget_t::Parameter objects.
template <typename... Widget_t>
[[nodiscard]] auto stuple(typename Widget_t::Parameters... p)
    -> std::unique_ptr<STuple<Widget_t...>>
{
    return std::make_unique<STuple<Widget_t...>>(std::move(p)...);
}

/// Build an STuple from existing Widgets.
template <typename... Widget_t>
[[nodiscard]] auto stuple(std::unique_ptr<Widget_t>... widget_ptrs)
    -> std::unique_ptr<STuple<Widget_t...>>
{
    return std::make_unique<STuple<Widget_t...>>(std::move(widget_ptrs)...);
}

}  // namespace ox
#endif  // TERMOX_WIDGET_TUPLE_HPP
