#ifndef PAINTER_GLYPH_HPP
#define PAINTER_GLYPH_HPP
#include "painter/brush.hpp"
#include <array>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>

namespace cppurses {

class Glyph {
   public:
    using Length_error = std::runtime_error;

    Glyph() = default;

    // Implcit conversions are allowed.
    template <typename... Attributes>
    Glyph(char symbol, Attributes&&... attrs)  // NOLINT
        : brush_{std::forward<Attributes>(attrs)...}, symbol_{{symbol, '\0'}} {}

    template <typename... Attributes>
    Glyph(const char* symbol, Attributes&&... attrs)  // NOLINT
        : brush_{std::forward<Attributes>(attrs)...} {
        this->set_symbol(symbol);
    }

    template <typename... Attributes>
    Glyph(const std::string& symbol, Attributes&&... attrs)  // NOLINT
        : brush_{std::forward<Attributes>(attrs)...} {
        this->set_symbol(symbol);
    }

    Glyph(char symbol, Brush b) : brush_{std::move(b)} {
        this->set_symbol(symbol);
    }

    Glyph(const char* symbol, Brush b) : brush_{std::move(b)} {
        this->set_symbol(symbol);
    }

    Glyph(const std::string& symbol, Brush b) : brush_{std::move(b)} {
        this->set_symbol(symbol);
    }

    void set_symbol(char symbol);
    void set_symbol(const char* symbol);
    void set_symbol(const std::string& symbol);

    std::string str() const;
    const char* c_str() const;
    operator std::string() const { return this->str(); }  // NOLINT

    void set_brush(const Brush& brush) { brush_ = brush; }

    const Brush& brush() const { return brush_; }
    Brush& brush() { return brush_; }

   private:
    Brush brush_;
    std::array<char, 5> symbol_{"\0\0\0\0"};
};

bool operator==(const Glyph& lhs, const Glyph& rhs);

bool operator!=(const Glyph& lhs, const Glyph& rhs);

std::ostream& operator<<(std::ostream& os, const Glyph& g);

}  // namespace cppurses
#endif  // PAINTER_GLYPH_HPP
