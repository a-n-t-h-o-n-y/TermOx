#ifndef TERMOX_WIDGET_ARRAY_HPP
#define TERMOX_WIDGET_ARRAY_HPP
#include <array>
#include <cstddef>
#include <functional>
#include <utility>

#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/stack.hpp>
#include <termox/widget/layouts/vertical.hpp>

namespace ox {

/// Homogeneous collection of Widgets within a Layout_t<Widget_t>.
/** Depends on Layout_t::make_child(args...) to construct the Widget_t. */
template <typename Layout_t, std::size_t N>
class Array : public Layout_t {
   public:
    /// \p args will be copied into each Widget's constructor call.
    template <typename... Args>
    explicit Array(Args&&... args)
        : refs_{create_n_children(std::make_index_sequence<N>(),
                                  std::forward<Args>(args)...)}
    {}

    explicit Array(
        std::array<typename Layout_t::Child_t::Parameters, N> parameters)
        : refs_{create_n_children(std::make_index_sequence<N>(),
                                  std::move(parameters))}
    {}

    explicit Array(
        std::array<std::unique_ptr<typename Layout_t::Child_t>, N> widget_ptrs)
        : refs_{create_n_children(std::make_index_sequence<N>(),
                                  std::move(widget_ptrs))}
    {}

   public:
    /// Get child by index.
    template <std::size_t Index>
    [[nodiscard]] auto get() -> auto&
    {
        return std::get<Index>(refs_).get();
    }

    /// Get child by index.
    template <std::size_t Index>
    [[nodiscard]] auto get() const -> auto const&
    {
        return std::get<Index>(refs_).get();
    }

   private:
    using References =
        std::array<std::reference_wrapper<typename Layout_t::Child_t>, N>;
    References refs_;

   private:
    template <std::size_t... Is, typename... Args>
    [[nodiscard]] auto create_n_children(std::index_sequence<Is...>,
                                         Args&&... args) -> References
    {
        return {this->insert_child(std::make_unique<typename Layout_t::Child_t>(
                                       std::forward<Args>(args)...),
                                   Is)...};
    }

    template <std::size_t... Is>
    [[nodiscard]] auto create_n_children(
        std::index_sequence<Is...>,
        std::array<typename Layout_t::Child_t::Parameters, N> parameters)
        -> References
    {
        return {this->insert_child(std::make_unique<typename Layout_t::Child_t>(
                                       std::move(parameters[Is])),
                                   Is)...};
    }

    template <std::size_t... Is>
    [[nodiscard]] auto create_n_children(
        std::index_sequence<Is...>,
        std::array<std::unique_ptr<typename Layout_t::Child_t>, N> widget_ptrs)
        -> References
    {
        return {this->insert_child(std::move(widget_ptrs[Is]), Is)...};
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

/// Build a default constructed Array unique_ptr.
template <typename Layout_t, std::size_t N>
[[nodiscard]] auto array() -> std::unique_ptr<Array<Layout_t, N>>
{
    return std::make_unique<Array<Layout_t, N>>();
}

/// Build an Array unique_ptr from Layout_t::Child_t::Parameter objects.
template <typename Layout_t, std::size_t N>
[[nodiscard]] auto array(
    std::array<typename Layout_t::Child_t::Parameters, N> parameters)
    -> std::unique_ptr<Array<Layout_t, N>>
{
    return std::make_unique<Array<Layout_t, N>>(std::move(parameters));
}

/// Build an Array from existing Widgets.
template <typename Layout_t, std::size_t N>
[[nodiscard]] auto array(
    std::array<std::unique_ptr<typename Layout_t::Child_t::Parameters>, N>
        parameter_ptrs) -> std::unique_ptr<Array<Layout_t, N>>
{
    return std::make_unique<Array<Layout_t, N>>(std::move(parameter_ptrs));
}

template <typename Widget_t, std::size_t N>
using VArray = Array<layout::Vertical<Widget_t>, N>;

/// Build a default constructed VArray unique_ptr.
template <typename Widget_t, std::size_t N>
[[nodiscard]] auto varray() -> std::unique_ptr<VArray<Widget_t, N>>
{
    return std::make_unique<VArray<Widget_t, N>>();
}

/// Build a VArray unique_ptr from Layout_t::Child_t::Parameter objects.
template <typename Widget_t, std::size_t N>
[[nodiscard]] auto varray(
    std::array<typename Widget_t::Parameters, N> parameters)
    -> std::unique_ptr<VArray<Widget_t, N>>
{
    return std::make_unique<VArray<Widget_t, N>>(std::move(parameters));
}

/// Build a VArray from existing Widgets.
template <typename Widget_t, std::size_t N>
[[nodiscard]] auto varray(std::array<std::unique_ptr<Widget_t>, N> widget_ptrs)
    -> std::unique_ptr<VArray<Widget_t, N>>
{
    return std::make_unique<VArray<Widget_t, N>>(std::move(widget_ptrs));
}

template <typename Widget_t, std::size_t N>
using HArray = Array<layout::Horizontal<Widget_t>, N>;

/// Build a default constructed HArray unique_ptr.
template <typename Widget_t, std::size_t N>
[[nodiscard]] auto harray() -> std::unique_ptr<HArray<Widget_t, N>>
{
    return std::make_unique<HArray<Widget_t, N>>();
}

/// Build an HArray unique_ptr from Layout_t::Child_t::Parameter objects.
template <typename Widget_t, std::size_t N>
[[nodiscard]] auto harray(
    std::array<typename Widget_t::Parameters, N> parameters)
    -> std::unique_ptr<HArray<Widget_t, N>>
{
    return std::make_unique<HArray<Widget_t, N>>(std::move(parameters));
}

/// Build an HArray from existing Widgets.
template <typename Widget_t, std::size_t N>
[[nodiscard]] auto harray(std::array<std::unique_ptr<Widget_t>, N> widget_ptrs)
    -> std::unique_ptr<HArray<Widget_t, N>>
{
    return std::make_unique<HArray<Widget_t, N>>(std::move(widget_ptrs));
}

template <typename Widget_t, std::size_t N>
using SArray = Array<layout::Stack<Widget_t>, N>;

/// Build a default constructed SArray unique_ptr.
template <typename Widget_t, std::size_t N>
[[nodiscard]] auto sarray() -> std::unique_ptr<SArray<Widget_t, N>>
{
    return std::make_unique<SArray<Widget_t, N>>();
}

/// Build an SArray unique_ptr from Layout_t::Child_t::Parameter objects.
template <typename Widget_t, std::size_t N>
[[nodiscard]] auto sarray(
    std::array<typename Widget_t::Parameters, N> parameters)
    -> std::unique_ptr<SArray<Widget_t, N>>
{
    return std::make_unique<SArray<Widget_t, N>>(std::move(parameters));
}

/// Build an SArray from existing Widgets.
template <typename Widget_t, std::size_t N>
[[nodiscard]] auto sarray(std::array<std::unique_ptr<Widget_t>, N> widget_ptrs)
    -> std::unique_ptr<SArray<Widget_t, N>>
{
    return std::make_unique<SArray<Widget_t, N>>(std::move(widget_ptrs));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_ARRAY_HPP
