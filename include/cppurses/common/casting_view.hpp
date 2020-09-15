#ifndef CPPURSES_COMMON_CASTING_VIEW_HPP
#define CPPURSES_COMMON_CASTING_VIEW_HPP
#include <cstddef>

#include <cppurses/common/transform_view.hpp>

namespace cppurses::detail {

template <typename Element>
inline auto constexpr downcast_fn =
    [](auto& widg_ptr) -> Element& { return static_cast<Element&>(*widg_ptr); };

}  // namespace cppurses::detail

namespace cppurses {

/// Casts each element in Container to a reference to Element type.
template <typename Element, typename Container>
using Casting_view =
    Transform_view<Container, decltype(detail::downcast_fn<Element>)>;

/// For template argument deduction on Container.
template <typename Element, typename Container>
auto casting_view(Container& container) -> Casting_view<Element, Container>
{
    return Casting_view<Element, Container>{container,
                                            detail::downcast_fn<Element>};
}

}  // namespace cppurses
#endif  // CPPURSES_COMMON_CASTING_VIEW_HPP
