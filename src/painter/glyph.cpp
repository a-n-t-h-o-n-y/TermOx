#include <painter/glyph.hpp>

#include <cstring>
#include <ostream>

namespace cppurses {

const char* Glyph::c_str() const {
    return symbol_.data();
}

std::string Glyph::str() const {
    return std::string(this->c_str());
}

void Glyph::set_symbol(char symbol) {
    symbol_[0] = symbol;
    symbol_[1] = '\0';
}

void Glyph::set_symbol(const char* symbol) {
    if (std::strlen(symbol) > 4) {
        throw Glyph::Length_error(
            "Glyph::set_symbol(const char*) - Symbol too large for Glyph.");
    }
    std::strncpy(symbol_.data(), symbol, 5);
}

void Glyph::set_symbol(const std::string& symbol) {
    if (symbol.size() > 4) {
        throw Glyph::Length_error(
            "Glyph::set_symbol(const string&) - Symbol too large for Glyph.");
    }
    std::strncpy(symbol_.data(), symbol.c_str(), 5);
}

bool operator==(const Glyph& lhs, const Glyph& rhs) {
    return ((std::strcmp(lhs.c_str(), rhs.c_str()) == 0) &&
            (lhs.brush() == rhs.brush()));
}

bool operator!=(const Glyph& lhs, const Glyph& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const Glyph& g) {
    return os << static_cast<std::string>(g);
}

}  // namespace cppurses
