#ifndef PAINTER_GLYPH_HPP
#define PAINTER_GLYPH_HPP
#include <cppurses/painter/brush.hpp>

namespace cppurses {

struct Glyph {
    // Data
    wchar_t symbol{L' '};
    Brush brush;

    // Methods
    Glyph() = default;

    Glyph(wchar_t sym, Brush b);

    template <typename... Attributes>
    Glyph(wchar_t sym, Attributes&&... attrs)
        : symbol{sym}, brush{std::forward<Attributes>(attrs)...} {}

    operator wchar_t() const;
};

bool operator==(const Glyph& lhs, const Glyph& rhs);

bool operator!=(const Glyph& lhs, const Glyph& rhs);

}  // namespace cppurses
#endif  // PAINTER_GLYPH_HPP
