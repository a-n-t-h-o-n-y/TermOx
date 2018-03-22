#include <cppurses/painter/glyph.hpp>

#include <utility>

namespace cppurses {
Glyph::Glyph(wchar_t sym, Brush b) : symbol{sym}, brush{std::move(b)} {}

Glyph::operator wchar_t() const {
    return symbol;
}

bool operator==(const Glyph& lhs, const Glyph& rhs) {
    return (lhs.symbol == rhs.symbol) && (lhs.brush == rhs.brush);
}

bool operator!=(const Glyph& lhs, const Glyph& rhs) {
    return !(lhs == rhs);
}

}  // namespace cppurses
