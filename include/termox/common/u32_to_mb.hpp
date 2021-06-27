#ifndef TERMOX_COMMON_U32_TO_MB_HPP
#define TERMOX_COMMON_U32_TO_MB_HPP
#include <string>
#include <string_view>

namespace ox {

/// char32_t to Multi-byte char conversion.
/** Depends on the currently set clocale to transform the bytes. */
[[nodiscard]] auto u32_to_mb(char32_t c) -> std::string;

/// char32_t string to Multi-byte string conversion.
/** Depends on the currently set clocale to transform the bytes. */
[[nodiscard]] auto u32_to_mb(std::u32string_view sv) -> std::string;

}  // namespace ox
#endif  // TERMOX_COMMON_U32_TO_MB_HPP
