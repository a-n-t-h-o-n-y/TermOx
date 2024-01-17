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

/// Opposite Layout type, but ::type is a template.
/** Not perfect, the passed in Layout_t has to be a complete type. Helpful for
 *  template template parameters. */
template <typename Layout_t>
struct Opposite_template;

template <typename Child_t>
struct Opposite_template<layout::Horizontal<Child_t>> {
    template <typename T>
    using type = layout::Vertical<T>;
};

template <typename Child_t>
struct Opposite_template<layout::Vertical<Child_t>> {
    template <typename T>
    using type = layout::Horizontal<T>;
};

// Can't have a Opposite_template_t, can't deduce type in this context.

}  // namespace ox::layout
#endif  // TERMOX_WIDGET_LAYOUTS_OPPOSITE_HPP
