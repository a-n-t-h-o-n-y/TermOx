#ifndef DETAIL_PLACEHOLDER_TEMPLATE_HPP
#define DETAIL_PLACEHOLDER_TEMPLATE_HPP

#include <functional>
#include <type_traits>

namespace sig {

// Allows you to bind a variable number of std::placeholder arguments to a
// std::bind call. Used in Connection_impl::bind_connection().
template <int>
struct Placeholder_template {};

}  // namespace sig

namespace std {

template <int N>
struct is_placeholder<sig::Placeholder_template<N>>
    : public integral_constant<int, N + 1> {};

}  // namespace std

#endif  // DETAIL_PLACEHOLDER_TEMPLATE_HPP
