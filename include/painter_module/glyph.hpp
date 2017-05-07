#ifndef GLYPH_HPP
#define GLYPH_HPP

#include "brush.hpp"
#include "color.hpp"

#include <array>
// #include <stdexcept>
#include <string>
#include <utility>
#include <ostream>
#include <cstring>

namespace twf {

// using Glyph_error = std::runtime_error;

class Glyph {
   public:
    Glyph() = default;

    template <typename... Attributes>
    Glyph(const char& symbol, Attributes&&... attrs)
        : brush_{std::forward<Attributes>(attrs)...}, symbol_{{symbol, '\0'}} {}

    template <typename... Attributes>
    Glyph(const char* symbol, Attributes&&... attrs)
        : brush_{std::forward<Attributes>(attrs)...} {
            this->set_symbol(symbol);
        }

    template <typename... Attributes>
    Glyph(const std::string& symbol, Attributes&&... attrs)
        : brush_{std::forward<Attributes>(attrs)...} {
            this->set_symbol(symbol);
        }

    operator std::string() const { return this->str(); }
    const char* c_str() const;
    std::string str() const;
    // char32_t get_char() const { return symbol_; }

    void set_symbol(char symbol) {
        symbol_[0] = symbol;
        symbol_[1] = '\0';
    }
    void set_symbol(const char* symbol) { std::strcpy(symbol_.data(), symbol); }
    void set_symbol(const std::string& symbol) {
        std::strcpy(symbol_.data(), symbol.c_str());
    }

    void set_brush(const Brush& brush) { brush_ = brush; }
    Brush& brush() { return brush_; }
    const Brush& brush() const { return brush_; }

    friend bool operator==(const Glyph& x, const Glyph& y) {
        return ((std::strcmp(x.symbol_.data(), y.symbol_.data()) == 0) &&
                (x.brush_ == y.brush_));
    }

    friend bool operator!=(const Glyph& x, const Glyph& y) { return !(x == y); }

   private:
    Brush brush_;
    // char32_t symbol_{U' '};
    std::array<char, 5> symbol_{"\0\0\0\0"};
    // mutable std::array<char, 5> symbol_c_str_{"\0\0\0\0"};

    // char32_t string_to_wchar(const std::string& s);
};
inline std::ostream& operator<<(std::ostream& os, const Glyph& g) {
    return os << static_cast<std::string>(g);
}

}  // namespace twf
#endif  // GLYPH_HPP
