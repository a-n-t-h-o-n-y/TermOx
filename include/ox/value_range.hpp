#pragma once
#include <concepts>
namespace ox {
template <typename T>
concept Numeric =
    (std::integral<T> || std::floating_point<T>) && !std::same_as<T, bool> &&
    !std::same_as<T, char> && !std::same_as<T, wchar_t> && !std::same_as<T, char8_t> &&
    !std::same_as<T, char16_t> && !std::same_as<T, char32_t>;
/** Finite inclusive bounds; positive increment. Values clamp without quantization. */
template <Numeric T>
struct BasicValueRange {
    T minimum = 0;
    T maximum = 100;
    T step = 1;
    auto operator==(BasicValueRange const&) const -> bool = default;
};
using ValueRange = BasicValueRange<double>;
}  // namespace ox
