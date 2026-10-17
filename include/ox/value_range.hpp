/** @file
 * Typed numeric ranges, clamping, and stepping.
 */
#pragma once
#include <concepts>
namespace ox {
/** Integral or floating-point arithmetic value type, excluding bool and character types. */
template <typename T>
concept Numeric =
    (std::integral<T> || std::floating_point<T>) && !std::same_as<T, bool> &&
    !std::same_as<T, char> && !std::same_as<T, wchar_t> && !std::same_as<T, char8_t> &&
    !std::same_as<T, char16_t> && !std::same_as<T, char32_t>;
/** Finite inclusive bounds; positive increment. Values clamp without quantization. */
template <Numeric T>
struct BasicValueRange {
    /** Inclusive finite lower bound. */
    T minimum = 0;
    /** Inclusive finite upper bound; must not be less than minimum. */
    T maximum = 100;
    /** Positive finite increment used by keyboard stepping. */
    T step = 1;
    /** Compare inclusive bounds and step exactly. */
    auto operator==(BasicValueRange const&) const -> bool = default;
};
/** Double-precision inclusive range used by Slider and ProgressBar. */
using ValueRange = BasicValueRange<double>;
}  // namespace ox
