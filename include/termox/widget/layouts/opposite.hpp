#ifndef TERMOX_WIDGET_LAYOUTS_OPPOSITE_HPP
#define TERMOX_WIDGET_LAYOUTS_OPPOSITE_HPP
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>

namespace ox::layout {

/// Given a Linear_layout type, it will provide a `type` member alias to the
/// opposite layout type. Vertical -> Horizontal : Horizontal -> Vertical
template <typename Layout_t>
struct Opposite;

template <typename Child_t>
struct Opposite<layout::Horizontal<Child_t>> {
    using type = layout::Vertical<Child_t>;
};

template <typename Child_t>
struct Opposite<layout::Vertical<Child_t>> {
    using type = layout::Horizontal<Child_t>;
};

template <typename Layout_t>
using Opposite_t = typename Opposite<Layout_t>::type;

}  // namespace ox::layout
#endif  // TERMOX_WIDGET_LAYOUTS_OPPOSITE_HPP
