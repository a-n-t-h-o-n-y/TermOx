#ifndef CPPURSES_PAINTER_UTILITY_WCHAR_TO_BYTES_HPP
#define CPPURSES_PAINTER_UTILITY_WCHAR_TO_BYTES_HPP
#include <string>

namespace cppurses {
namespace utility {

std::string wchar_to_bytes(wchar_t ch);

std::string wchar_to_bytes(std::wstring w_str);

}  // namespace utility
}  // namespace cppurses

#endif  // CPPURSES_PAINTER_UTILITY_WCHAR_TO_BYTES_HPP
