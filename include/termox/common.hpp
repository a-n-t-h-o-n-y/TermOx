#pragma once

#include <string>
#include <string_view>

namespace ox {

/**
 * @brief Implements the 'overload' pattern, useful for creating lambda overload
 * sets.
 */
template <class... Ts>
struct Overload : Ts... {
    using Ts::operator()...;
};

/**
 * @brief char32_t to Multi-byte char conversion.
 *
 * Depends on the currently set clocale to transform the bytes.
 *
 * @param c The char32_t to convert.
 * @return std::string The resulting multi-byte string.
 * @throws std::runtime_error if there is an error during conversion.
 */
[[nodiscard]] auto u32_to_mb(char32_t c) -> std::string;

/**
 * @brief char32_t string to Multi-byte string conversion.
 *
 * Depends on the currently set clocale to transform the bytes.
 *
 * @param sv The char32_t string to convert.
 * @return std::string The resulting multi-byte string.
 * @throws std::runtime_error if there is an error during conversion.
 */
[[nodiscard]] auto u32_to_mb(std::u32string_view sv) -> std::string;

}  // namespace ox