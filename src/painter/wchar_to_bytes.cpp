#include <cppurses/painter/utility/wchar_to_bytes.hpp>

#include <codecvt>
#include <locale>
#include <string>

namespace cppurses {
namespace utility {

std::string wchar_to_bytes(wchar_t ch) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string result{converter.to_bytes(ch)};
    return result;
}

std::string wchar_to_bytes(std::wstring w_str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string result{converter.to_bytes(w_str)};
    return result;
}

}  // namespace utility
}  // namespace cppurses
