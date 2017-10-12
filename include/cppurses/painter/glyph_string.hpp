#ifndef PAINTER_GLYPH_STRING_HPP
#define PAINTER_GLYPH_STRING_HPP

#include "painter/glyph.hpp"
#include <codecvt>
#include <initializer_list>
#include <locale>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace cppurses {

class Glyph_string : private std::vector<Glyph> {
   public:
    Glyph_string() = default;

    // Used as 'Until the end of the string'
    static const std::size_t npos = -1;

    // Iterators
    template <typename InputIterator>
    Glyph_string(InputIterator first, InputIterator last)
        : vector<Glyph>::vector(first, last) {}

    template <typename... Attributes>
    Glyph_string(const std::string& symbols, Attributes&&... attrs)  // NOLINT
        : vector<Glyph>::vector() {
        this->append(symbols, std::forward<Attributes>(attrs)...);
    }

    template <typename... Attributes>
    Glyph_string(const char* symbols, Attributes&&... attrs) {  // NOLINT
        this->append(symbols, std::forward<Attributes>(attrs)...);
    }

    template <typename... Attributes>
    Glyph_string(char symbol, Attributes&&... attrs) {  // NOLINT
        this->append(Glyph{symbol, std::forward<Attributes>(attrs)...});
    }

    template <typename... Attributes>
    Glyph_string(const Glyph& glyph, Attributes&&... attrs) {  // NOLINT
        this->append(glyph, std::forward<Attributes>(attrs)...);
    }

    template <typename... Attributes>
    Glyph_string(const std::initializer_list<Glyph>& glyphs,
                 Attributes&&... attrs) {
        this->reserve(glyphs.size());
        for (const Glyph& g : glyphs) {
            this->append(g, std::forward<Attributes>(attrs)...);
        }
    }

    operator std::string() const;  // NOLINT
    std::string str() const;
    size_type length() const;

    Glyph_string& operator+=(const Glyph& glyph);
    Glyph_string operator+(const Glyph_string& gs);

    template <typename... Attributes>
    Glyph_string& append(const Glyph& symbol, Attributes&&... attrs) {
        this->push_back(symbol);
        this->back().brush().add_attributes(std::forward<Attributes>(attrs)...);
        return *this;
    }

    // Multi-byte char compatibility.
    // Converts to char32 string, then converts each char32 to a byte string
    template <typename... Attributes>
    Glyph_string& append(const char* symbols, Attributes&&... attrs) {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
        std::u32string wide_string = converter.from_bytes(symbols);
        reserve(this->size() + wide_string.size());
        for (char32_t sym : wide_string) {
            this->append(Glyph{converter.to_bytes(sym)},
                         std::forward<Attributes>(attrs)...);
        }
        return *this;
    }

    template <typename... Attributes>
    Glyph_string& append(const std::string& symbols, Attributes&&... attrs) {
        return this->append(symbols.c_str(),
                            std::forward<Attributes>(attrs)...);
    }

    template <typename... Attributes>
    Glyph_string& append(const Glyph_string& gs, Attributes&&... attrs) {
        for (const Glyph& glyph : gs) {
            this->append(glyph, std::forward<Attributes>(attrs)...);
        }
        return *this;
    }

    template <typename... Attributes>
    void add_attributes(Attributes&&... attrs) {
        for (auto& glyph : *this) {
            glyph.brush().add_attributes(std::forward<Attributes>(attrs)...);
        }
    }

    void remove_attribute(Attribute attr);

    // Functions from std::vector<Glyph>
    using std::vector<Glyph>::value_type;
    using std::vector<Glyph>::allocator_type;
    using std::vector<Glyph>::size_type;
    using std::vector<Glyph>::difference_type;
    using std::vector<Glyph>::reference;
    using std::vector<Glyph>::const_reference;
    using std::vector<Glyph>::pointer;
    using std::vector<Glyph>::const_pointer;
    using std::vector<Glyph>::iterator;
    using std::vector<Glyph>::const_iterator;
    using std::vector<Glyph>::reverse_iterator;
    using std::vector<Glyph>::const_reverse_iterator;

    using std::vector<Glyph>::operator=;
    using std::vector<Glyph>::operator[];
    using std::vector<Glyph>::size;
    using std::vector<Glyph>::assign;
    using std::vector<Glyph>::get_allocator;
    using std::vector<Glyph>::at;
    using std::vector<Glyph>::front;
    using std::vector<Glyph>::back;
    using std::vector<Glyph>::data;
    using std::vector<Glyph>::begin;
    using std::vector<Glyph>::cbegin;
    using std::vector<Glyph>::end;
    using std::vector<Glyph>::cend;
    using std::vector<Glyph>::rbegin;
    using std::vector<Glyph>::crbegin;
    using std::vector<Glyph>::rend;
    using std::vector<Glyph>::crend;
    using std::vector<Glyph>::empty;
    using std::vector<Glyph>::max_size;
    using std::vector<Glyph>::reserve;
    using std::vector<Glyph>::capacity;
    using std::vector<Glyph>::shrink_to_fit;
    using std::vector<Glyph>::clear;
    using std::vector<Glyph>::insert;
    using std::vector<Glyph>::erase;
    using std::vector<Glyph>::push_back;
    using std::vector<Glyph>::pop_back;
    using std::vector<Glyph>::resize;
    using std::vector<Glyph>::swap;
};

bool operator==(const Glyph_string& x, const Glyph_string& y);
bool operator!=(const Glyph_string& x, const Glyph_string& y);
std::ostream& operator<<(std::ostream& os, const Glyph_string& gs);

}  // namespace cppurses
#endif  // PAINTER_GLYPH_STRING_HPP
