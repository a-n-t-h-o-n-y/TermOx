#ifndef TERMOX_WIDGET_LAYOUTS_VERTICAL_HPP
#define TERMOX_WIDGET_LAYOUTS_VERTICAL_HPP
#include <cstddef>
#include <memory>
#include <utility>

#include "detail/linear_layout.hpp"

namespace ox::layout::v_detail {

struct Get_area {
    auto operator()(std::size_t primary, std::size_t secondary) const -> Area
    {
        return {secondary, primary};
    }
};

struct Get_point {
    auto operator()(std::size_t primary, std::size_t secondary) const -> Point
    {
        return {secondary, primary};
    }
};

struct Primary_policy {
    auto operator()(Widget const& w) const -> Size_policy const&
    {
        return w.height_policy;
    }
};

struct Primary_length {
    auto operator()(Widget const& w) const -> std::size_t { return w.height(); }
};

struct Primary_offset {
    auto operator()(Widget const& w) const -> std::size_t
    {
        return w.inner_y();
    }
};

struct Secondary_policy {
    auto operator()(Widget const& w) const -> Size_policy const&
    {
        return w.width_policy;
    }
};

struct Secondary_length {
    auto operator()(Widget const& w) const -> std::size_t { return w.width(); }
};

struct Secondary_offset {
    auto operator()(Widget const& w) const -> std::size_t
    {
        return w.inner_x();
    }
};

using Primary = detail::Dimension_parameters<Primary_policy,
                                             Primary_length,
                                             Primary_offset,
                                             Policy_direction::Vertical>;

using Secondary = detail::Dimension_parameters<Secondary_policy,
                                               Secondary_length,
                                               Secondary_offset,
                                               Policy_direction::Horizontal>;

using Vertical_parameters =
    detail::Linear_layout_parameters<Primary, Secondary, Get_area, Get_point>;

}  // namespace ox::layout::v_detail

namespace ox::layout {

template <typename Child_t = Widget>
using Vertical = detail::Linear_layout<Child_t, v_detail::Vertical_parameters>;

/// Helper function to create an instance.
template <typename Widget_t = Widget, typename... Args>
auto vertical(Args&&... args) -> std::unique_ptr<Vertical<Widget_t>>
{
    return std::make_unique<Vertical<Widget_t>>(std::forward<Args>(args)...);
}

/// True if the given type is a layout::Vertical.
template <typename Layout_t>
inline constexpr bool is_vertical_v = false;

/// True if the given type is a layout::Vertical.
template <typename Child_t>
inline constexpr bool is_vertical_v<layout::Vertical<Child_t>> = true;

}  // namespace ox::layout
#endif  // TERMOX_WIDGET_LAYOUTS_VERTICAL_HPP
