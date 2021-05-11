#include <termox/widget/widgets/detail/nearly_equal.hpp>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace {

/// Type punning to reinterpret a value of Input_t as Result_t.
template <typename Result_t, typename Input_t>
[[nodiscard]] auto reinterpret(Input_t const& value) -> Result_t
{
    static_assert(std::is_trivially_copyable<Result_t>::value &&
                      std::is_trivially_copyable<Input_t>::value,
                  "reinterpret<Result_t, Input_t>(): Type Errors");
    static_assert(sizeof(Result_t) == sizeof(Input_t),
                  "reinterpret<Result_t, Input_t>(): Type size mismatch.");
    auto result = Result_t{};
    std::memcpy(&result, &value, sizeof(result));
    return result;
}

}  // namespace

namespace ox::detail {

auto is_nearly_equal(double a, double b) -> bool
{
    auto const max_ulps_difference = 1;
    return std::abs(::reinterpret<std::int64_t>(a) -
                    ::reinterpret<std::int64_t>(b)) <= max_ulps_difference;
}

auto is_nearly_whole(double value) -> bool
{
    return is_nearly_equal(value, std::ceil(value));
}

auto ceil_if_nearly_whole(double value) -> double
{
    if (is_nearly_whole(value))
        value = std::ceil(value);
    return value;
}

}  // namespace ox::detail
