#ifndef PLACEHOLDER_TEMPLATE_HPP
#define PLACEHOLDER_TEMPLATE_HPP

#include <functional>
#include <type_traits>

namespace mcurses
{

template <int>
struct placeholder_template
{};

} // namespace mcurses

namespace std
{

template <int N>
struct is_placeholder<mcurses::placeholder_template<N>>
:public integral_constant<int, N+1>
{};

} // namespace std

#endif // PLACEHOLDER_TEMPLATE_HPP