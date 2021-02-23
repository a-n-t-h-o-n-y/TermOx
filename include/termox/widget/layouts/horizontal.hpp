#ifndef TERMOX_WIDGET_LAYOUTS_HORIZONTAL_HPP
#define TERMOX_WIDGET_LAYOUTS_HORIZONTAL_HPP
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

#include "detail/linear_layout.hpp"

namespace ox::layout::h_detail {

struct Get_area {
    [[nodiscard]] auto operator()(int primary, int secondary) const -> Area
    {
        return {primary, secondary};
    }
};

struct Get_point {
    [[nodiscard]] auto operator()(int primary, int secondary) const -> Point
    {
        return {primary, secondary};
    }
};

struct Primary_policy {
    [[nodiscard]] auto operator()(Widget const& w) const -> Size_policy const&
    {
        return w.width_policy;
    }
};

struct Primary_length {
    [[nodiscard]] auto operator()(Widget const& w) const -> int
    {
        return w.width();
    }
};

struct Primary_offset {
    [[nodiscard]] auto operator()(Widget const& w) const -> int
    {
        return w.inner_x();
    }
};

struct Secondary_policy {
    [[nodiscard]] auto operator()(Widget const& w) const -> Size_policy const&
    {
        return w.height_policy;
    }
};

struct Secondary_length {
    [[nodiscard]] auto operator()(Widget const& w) const -> int
    {
        return w.height();
    }
};

struct Secondary_offset {
    [[nodiscard]] auto operator()(Widget const& w) const -> int
    {
        return w.inner_y();
    }
};

using Primary = detail::Dimension_parameters<Primary_policy,
                                             Primary_length,
                                             Primary_offset,
                                             Policy_direction::Horizontal>;

using Secondary = detail::Dimension_parameters<Secondary_policy,
                                               Secondary_length,
                                               Secondary_offset,
                                               Policy_direction::Vertical>;

using Horizontal_parameters =
    detail::Linear_layout_parameters<Primary, Secondary, Get_area, Get_point>;

}  // namespace ox::layout::h_detail

namespace ox::layout {

template <typename Child_t = Widget>
using Horizontal =
    detail::Linear_layout<Child_t, h_detail::Horizontal_parameters>;

/// Helper function to create an instance.
template <typename Widget_t = Widget, typename... Args>
[[nodiscard]] auto horizontal(Args&&... args)
    -> std::unique_ptr<Horizontal<Widget_t>>
{
    return std::make_unique<Horizontal<Widget_t>>(std::forward<Args>(args)...);
}

namespace detail {
template <typename Child_t>
auto is_horizontal_impl(layout::Horizontal<Child_t>&) -> std::true_type;

auto is_horizontal_impl(...) -> std::false_type;
}  // namespace detail

template <typename T>
using Is_horizontal = decltype(detail::is_horizontal_impl(std::declval<T&>()));

/// True if the given type is a layout::Horizontal type or derived from.
template <typename T>
inline bool constexpr is_horizontal_v = Is_horizontal<T>::value;

}  // namespace ox::layout
#endif  // TERMOX_WIDGET_LAYOUTS_HORIZONTAL_HPP
