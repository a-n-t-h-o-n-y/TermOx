#ifndef TERMOX_WIDGET_DETAIL_PIPE_UTILITY_HPP
#define TERMOX_WIDGET_DETAIL_PIPE_UTILITY_HPP
#include <memory>
#include <type_traits>

#include <termox/widget/widget.hpp>

namespace ox::pipe::detail {

/// Used to call operator| overload to create a new Range from filter predicate
template <typename Predicate>
class Filter_predicate {
   public:
    explicit Filter_predicate(Predicate p) : predicate{p} {}

   public:
    Predicate predicate;
};

/// Used to call operator| overload to create a new Range from filter predicate
template <typename W>
class Dynamic_filter_predicate {
   public:
    using Widget_t = W;
};

template <typename T>
struct is_widget_ptr : std::false_type {};

template <typename X>
struct is_widget_ptr<std::unique_ptr<X>> : std::is_base_of<ox::Widget, X> {};

/// True if T is a std::unique_ptr<> to a ox::Widget type.
template <typename T>
constexpr bool is_widget_ptr_v = is_widget_ptr<T>::value;

/// True if T is a Widget type.
template <typename T>
constexpr bool is_widget_v = std::is_base_of_v<Widget, std::decay_t<T>>;

/// True if T is a Widget type or a Widget pointer.
template <typename T>
constexpr bool is_widget_or_wptr =
    is_widget_v<T> || is_widget_ptr_v<std::decay_t<T>>;

}  // namespace ox::pipe::detail

namespace ox {

/// Return *x if x is a unique_ptr to a Widget type, otherwise x.
template <typename T>
[[nodiscard]] constexpr auto get(T& x) -> auto&
{
    if constexpr (::ox::pipe::detail::is_widget_ptr_v<T>)
        return *x;
    else
        return x;
}

}  // namespace ox
#endif  // TERMOX_WIDGET_DETAIL_PIPE_UTILITY_HPP
