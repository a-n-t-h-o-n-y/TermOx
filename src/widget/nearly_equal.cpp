#include <cmath>
#include <cppurses/widget/widgets/detail/nearly_equal.hpp>
#include <cstdint>
#include <cstdlib>

namespace cppurses {
namespace detail {

bool is_nearly_equal(double a, double b) {
    const auto max_ulps_difference = 1;
    return std::abs(reinterpret<std::int64_t>(a) -
                    reinterpret<std::int64_t>(b)) <= max_ulps_difference;
}

bool is_nearly_whole(double value) {
    return is_nearly_equal(value, std::ceil(value));
}

double ceil_if_nearly_whole(double value) {
    if (is_nearly_whole(value)) {
        value = std::ceil(value);
    }
    return value;
}

}  // namespace detail
}  // namespace cppurses
