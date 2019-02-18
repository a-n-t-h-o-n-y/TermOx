#include <cppurses/painter/glyph_string.hpp>

#include <iterator>
#include <algorithm>
#include <string>

#include <cppurses/painter/glyph.hpp>

namespace cppurses {

std::wstring Glyph_string::w_str() const {
    auto result = std::wstring{L""};
    for (const auto& glyph : *this) {
        result.push_back(glyph.symbol);
    }
    return result;
}

Glyph_string Glyph_string::operator+(const Glyph_string& gs) const {
    Glyph_string result{*this};
    result.append(gs);
    return result;
}

void Glyph_string::remove_attribute(Attribute attr) {
    for (Glyph& glyph : *this) {
        glyph.brush.remove_attributes(attr);
    }
}

bool operator==(const Glyph_string& x, const Glyph_string& y) {
    return std::equal(std::begin(x), std::end(x), std::begin(y), std::end(y));
}

}  // namespace cppurses
