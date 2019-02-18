#ifndef CPPURSES_PAINTER_UTILITY_WCHAR_TO_BYTES_HPP
#define CPPURSES_PAINTER_UTILITY_WCHAR_TO_BYTES_HPP
#include <string>

namespace cppurses {
namespace utility {

/// Return a string of char/bytes representing the wchar_t \p ch.
std::string wchar_to_bytes(wchar_t ch);

/// Return a string of char/bytes representing the wstring \p w_str.
std::string wchar_to_bytes(std::wstring w_str);

}  // namespace utility
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_UTILITY_WCHAR_TO_BYTES_HPP
