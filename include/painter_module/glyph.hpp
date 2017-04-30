#ifndef GLYPH_HPP
#define GLYPH_HPP

#include "brush.hpp"
#include "color.hpp"

#include <stdexcept>
#include <string>
#include <utility>
#include <ostream>

namespace twf {

using Glyph_error = std::runtime_error;
// class Glyph_error : public std::runtime_error {
//    public:
//     using std::runtime_error::runtime_error;
// };

class Glyph {
   public:
    Glyph() = default;

    template <typename... Attributes>
    Glyph(const char32_t& symbol, Attributes&&... attrs)
        : brush_{std::forward<Attributes>(attrs)...}, symbol_{symbol} {}

    // template <typename... Attributes>
    // Glyph(const char16_t& symbol, Attributes&&... attrs)
    //     : brush_{std::forward<Attributes>(attrs)...}, symbol_(symbol) {}

    // template <typename... Attributes>
    // Glyph(const char& symbol, Attributes&&... attrs)
    //     : brush_{std::forward<Attributes>(attrs)...}, symbol_(symbol) {}

    template <typename... Attributes>
    Glyph(const char* symbol, Attributes&&... attrs)
        : brush_{std::forward<Attributes>(attrs)...} {
        symbol_ = string_to_wchar(symbol);
    }

    template <typename... Attributes>
    Glyph(const std::string& symbol, Attributes&&... attrs)
        : brush_{std::forward<Attributes>(attrs)...} {
        symbol_ = string_to_wchar(symbol);
    }

    operator std::string() const { return this->str(); }
    std::string str() const;
    char32_t get_char() const { return symbol_; }

    void set_symbol(char32_t symbol) { symbol_ = symbol; }
    void set_symbol(const char* symbol) { symbol_ = string_to_wchar(symbol); }
    void set_symbol(const std::string& symbol) {
        symbol_ = string_to_wchar(symbol);
    }

    void set_brush(const Brush& brush) { brush_ = brush; }
    Brush& brush() { return brush_; }
    const Brush& brush() const { return brush_; }

    friend bool operator==(const Glyph& x, const Glyph& y) {
        return ((x.symbol_ == y.symbol_) && (x.brush_ == y.brush_));
    }

    friend bool operator!=(const Glyph& x, const Glyph& y) { return !(x == y); }

   private:
    Brush brush_;
    char32_t symbol_ = U' ';

    char32_t string_to_wchar(const std::string& s);
};

inline std::ostream& operator<<(std::ostream& os, const Glyph& g) {
    return os << static_cast<std::string>(g);
}

}  // namespace twf
#endif  // GLYPH_HPP
