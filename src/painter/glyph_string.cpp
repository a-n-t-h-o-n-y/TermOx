#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/utility/wchar_to_bytes.hpp>

#include <algorithm>
#include <codecvt>
#include <iterator>
#include <locale>
#include <ostream>
#include <string>

namespace cppurses {

Glyph_string::operator std::string() const {  // NOLINT
    return this->str();
}

std::string Glyph_string::str() const {
    const std::wstring wide_str{this->w_str()};
    std::string result = utility::wchar_to_bytes(wide_str);
    return result;
}

Glyph_string::operator std::wstring() const {  // NOLINT
    return this->w_str();
}

std::wstring Glyph_string::w_str() const {
    std::wstring result;
    for (const Glyph& g : *this) {
        result.push_back(g.symbol);
    }
    return result;
}

Glyph_string::size_type Glyph_string::length() const {
    return this->size();
}

Glyph_string& Glyph_string::operator+=(const Glyph& glyph) {
    return this->append(glyph);
}

Glyph_string Glyph_string::operator+(const Glyph_string& gs) const {
    Glyph_string result{*this};
    result.append(gs);
    return result;
}

void Glyph_string::remove_attribute(Attribute attr) {
    for (Glyph& glyph : *this) {
        glyph.brush.remove_attribute(attr);
    }
}

bool operator==(const Glyph_string& x, const Glyph_string& y) {
    return std::equal(std::begin(x), std::end(x), std::begin(y), std::end(y));
}

bool operator!=(const Glyph_string& x, const Glyph_string& y) {
    return !(x == y);
}

std::ostream& operator<<(std::ostream& os, const Glyph_string& gs) {
    return os << static_cast<std::string>(gs);
}

}  // namespace cppurses
