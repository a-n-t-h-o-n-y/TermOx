#ifndef CPPURSES_WIDGET_WIDGETS_DETAIL_NEARLY_EQUAL_HPP
#define CPPURSES_WIDGET_WIDGETS_DETAIL_NEARLY_EQUAL_HPP
#include <cstring>
#include <type_traits>

namespace cppurses {
namespace detail {

/// Type punning to reinterpret a value of U as T.
template <typename Result_t, typename Input_t>
Result_t reinterpret(const Input_t& value) {
    static_assert(std::is_trivially_copyable<Result_t>::value &&
                      std::is_trivially_copyable<Input_t>::value,
                  "reinterpret<Result_t, Input_t>(): Type Errors");
    static_assert(sizeof(Result_t) == sizeof(Input_t),
                  "reinterpret<Result_t, Input_t>(): Type size mismatch.");
    Result_t result;
    std::memcpy(&result, &value, sizeof(result));
    return result;
}

/// Return true if there are no floating point values between \p a and \p b.
bool is_nearly_equal(double a, double b);

/// Return true if \p value is nearly equal to the next greatest whole number.
bool is_nearly_whole(double value);

/// Round up towards the nearest whole number greater than \p value.
/** Only rounds up if there are no floating point values between \p value and
 *  the next whole number. */
double ceil_if_nearly_whole(double value);

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_DETAIL_NEARLY_EQUAL_HPP
