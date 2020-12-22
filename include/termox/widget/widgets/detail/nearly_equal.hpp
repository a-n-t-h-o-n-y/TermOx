#ifndef TERMOX_WIDGET_WIDGETS_DETAIL_NEARLY_EQUAL_HPP
#define TERMOX_WIDGET_WIDGETS_DETAIL_NEARLY_EQUAL_HPP
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <type_traits>

namespace ox::detail {

/// Type punning to reinterpret a value of U as T.
template <typename Result_t, typename Input_t>
auto reinterpret(Input_t const& value) -> Result_t
{
    static_assert(std::is_trivially_copyable<Result_t>::value and
                      std::is_trivially_copyable<Input_t>::value,
                  "reinterpret<Result_t, Input_t>(): Type Errors");
    static_assert(sizeof(Result_t) == sizeof(Input_t),
                  "reinterpret<Result_t, Input_t>(): Type size mismatch.");
    auto result = Result_t{};
    std::memcpy(&result, &value, sizeof(result));
    return result;
}

/// Return true if there are no floating point values between \p a and \p b.
inline auto is_nearly_equal(double a, double b) -> bool
{
    auto const max_ulps_difference = 1;
    return std::abs(reinterpret<std::int64_t>(a) -
                    reinterpret<std::int64_t>(b)) <= max_ulps_difference;
}

/// Return true if \p value is nearly equal to the next greatest whole number.
inline auto is_nearly_whole(double value) -> bool
{
    return is_nearly_equal(value, std::ceil(value));
}

/// Round up towards the nearest whole number greater than \p value.
/** Only rounds up if there are no floating point values between \p value and
 *  the next whole number. */
inline auto ceil_if_nearly_whole(double value) -> double
{
    if (is_nearly_whole(value))
        value = std::ceil(value);
    return value;
}

}  // namespace ox::detail
#endif  // TERMOX_WIDGET_WIDGETS_DETAIL_NEARLY_EQUAL_HPP
