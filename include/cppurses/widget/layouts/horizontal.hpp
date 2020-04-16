#ifndef CPPURSES_WIDGET_LAYOUTS_HORIZONTAL_HPP
#define CPPURSES_WIDGET_LAYOUTS_HORIZONTAL_HPP
#include <cstddef>

#include "detail/linear_layout.hpp"

namespace cppurses::layout {
namespace h_detail {

struct Get_area {
    auto operator()(std::size_t primary, std::size_t secondary) const -> Area
    {
        return {primary, secondary};
    }
};

struct Get_point {
    auto operator()(std::size_t primary, std::size_t secondary) const -> Point
    {
        return {primary, secondary};
    }
};

struct Primary_policy {
    auto operator()(Widget const& w) const -> Size_policy const&
    {
        return w.width_policy;
    }
};

struct Primary_length {
    auto operator()(Widget const& w) const -> std::size_t { return w.width(); }
};

struct Primary_offset {
    auto operator()(Widget const& w) const -> std::size_t
    {
        return w.inner_x();
    }
};

struct Secondary_policy {
    auto operator()(Widget const& w) const -> Size_policy const&
    {
        return w.height_policy;
    }
};

struct Secondary_length {
    auto operator()(Widget const& w) const -> std::size_t { return w.height(); }
};

struct Secondary_offset {
    auto operator()(Widget const& w) const -> std::size_t
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

}  // namespace h_detail

template <typename Child_t = Widget>
using Horizontal =
    detail::Linear_layout<Child_t, h_detail::Horizontal_parameters>;

}  // namespace cppurses::layout
#endif  // CPPURSES_WIDGET_LAYOUTS_HORIZONTAL_HPP
