#ifndef TERMOX_COMMON_COMMON_TYPE_HPP
#define TERMOX_COMMON_COMMON_TYPE_HPP
#include <type_traits>

#include <termox/widget/widget.hpp>

namespace ox {

/// If T and U are the same type, then return T, else return ox::Widget.
/** Useful in layout wrappers to provide a bit of convinience. */
template <typename T, typename U>
using Common_type = std::conditional_t<std::is_same_v<T, U>, T, ox::Widget>;

}  // namespace ox
#endif  // TERMOX_COMMON_COMMON_TYPE_HPP
