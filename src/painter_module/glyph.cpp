#include <painter_module/glyph.hpp>

#include <codecvt>
#include <locale>

namespace twf {

std::string Glyph::str() const {
    return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>()
        .to_bytes(symbol_);
}

char32_t Glyph::string_to_wchar(const std::string& s) {
    auto wstring = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>()
                       .from_bytes(s);
    if (wstring.size() == 1) {
        return wstring[0];
    }
    throw Glyph_error(
        "Glyph constructor: symbol must be printable as a single "
        "character");
}

}  // namespace twf
