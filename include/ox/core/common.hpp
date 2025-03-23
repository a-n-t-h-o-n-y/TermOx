#pragma once

#include <ranges>
#include <type_traits>

namespace ox {

/**
 * @brief Concept for an input range of T.
 */
template <typename R, typename T>
concept InputRangeOf =
    std::ranges::input_range<R> && std::same_as<std::ranges::range_value_t<R>, T>;

}  // namespace ox