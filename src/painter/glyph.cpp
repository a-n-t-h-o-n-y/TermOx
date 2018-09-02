#include <cppurses/painter/glyph.hpp>

#include <cppurses/painter/brush.hpp>

namespace cppurses {
Glyph::Glyph(wchar_t sym, const Brush& b) : symbol{sym}, brush{b} {}

bool operator==(const Glyph& lhs, const Glyph& rhs) {
    return (lhs.symbol == rhs.symbol) && (lhs.brush == rhs.brush);
}

bool operator!=(const Glyph& lhs, const Glyph& rhs) {
    return !(lhs == rhs);
}

}  // namespace cppurses
