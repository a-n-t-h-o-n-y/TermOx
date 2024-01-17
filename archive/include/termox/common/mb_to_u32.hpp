#ifndef TERMOX_COMMON_MB_TO_U32_HPP
#define TERMOX_COMMON_MB_TO_U32_HPP
#include <string>
#include <string_view>

namespace ox {

/// Multi-byte string to char32_t string conversion.
/** Depends on the currently set clocale to transform the bytes. */
[[nodiscard]] auto mb_to_u32(std::string_view sv) -> std::u32string;

}  // namespace ox
#endif  // TERMOX_COMMON_MB_TO_U32_HPP
