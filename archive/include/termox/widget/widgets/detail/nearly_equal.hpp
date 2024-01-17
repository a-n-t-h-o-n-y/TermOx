#ifndef TERMOX_WIDGET_WIDGETS_DETAIL_NEARLY_EQUAL_HPP
#define TERMOX_WIDGET_WIDGETS_DETAIL_NEARLY_EQUAL_HPP
namespace ox::detail {

/// Return true if there are no floating point values between \p a and \p b.
[[nodiscard]] auto is_nearly_equal(double a, double b) -> bool;

/// Return true if \p value is nearly equal to the next greatest whole number.
[[nodiscard]] auto is_nearly_whole(double value) -> bool;

/// Round up towards the nearest whole number greater than \p value.
/** Only rounds up if there are no floating point values between \p value and
 *  the next whole number. */
[[nodiscard]] auto ceil_if_nearly_whole(double value) -> double;

}  // namespace ox::detail
#endif  // TERMOX_WIDGET_WIDGETS_DETAIL_NEARLY_EQUAL_HPP
