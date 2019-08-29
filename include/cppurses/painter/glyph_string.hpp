#ifndef CPPURSES_PAINTER_GLYPH_STRING_HPP
#define CPPURSES_PAINTER_GLYPH_STRING_HPP
#include <codecvt>
#include <initializer_list>
#include <locale>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/utility/wchar_to_bytes.hpp>

namespace cppurses {

/// Holds a collection of Glyphs with a similar interface to std::string.
class Glyph_string : private std::vector<Glyph> {
   public:
    /// Default constructs an empty Glyph_string.
    Glyph_string() = default;

    /// Used to indicate 'Until the end of the string'.
    static const std::size_t npos = -1;

    /// Construct with iterators from any container providing Input Iterators.
    template <typename InputIterator>
    Glyph_string(InputIterator first, InputIterator last);

    /// Construct with \p symbols, each having given Attributes applied to them.
    template <typename... Attributes>
    Glyph_string(const std::string& symbols, Attributes&&... attrs);

    /// Construct with \p symbols, each having given Attributes applied to them.
    template <typename... Attributes>
    Glyph_string(const char* symbols, Attributes&&... attrs);

    /// Construct with \p symbol, having given Attributes applied to it.
    template <typename... Attributes>
    Glyph_string(wchar_t symbol, Attributes&&... attrs);

    /// Construct with \p symbols, each having given Attributes applied to them.
    template <typename... Attributes>
    Glyph_string(const wchar_t* symbols, Attributes&&... attrs);

    /// Construct with \p symbols, each having given Attributes applied to them.
    template <typename... Attributes>
    Glyph_string(const std::wstring& symbols, Attributes&&... attrs);

    /// Construct with \p glyph, adding given Attributes to it.
    template <typename... Attributes>
    Glyph_string(const Glyph& glyph, Attributes&&... attrs);

    /// Construct with a std::initializer_list of Glyphs, with \p attrs applied.
    template <typename... Attributes>
    Glyph_string(const std::initializer_list<Glyph>& glyphs,
                 Attributes&&... attrs);

    Glyph_string& operator=(const Glyph_string&) = default;

    /// Convert to a std::string, each Glyph being a char.
    std::string str() const { return utility::wchar_to_bytes(this->w_str()); }

    /// Convert to a std::wstring, each Glyph being a wchar_t.
    std::wstring w_str() const;

    /// Return the length in Glyphs of the Glyph_string.
    size_type length() const { return this->size(); }

    /// Compound concatenation assignment operator to append a Glyph.
    Glyph_string& operator+=(const Glyph& glyph) { return this->append(glyph); }

    /// Concatenation operator to append a Glyph_string.
    Glyph_string operator+(const Glyph_string& gs) const;

    /// Append single Glyph to the end of the Glyph_string w/given Attributes.
    template <typename... Attributes>
    Glyph_string& append(const Glyph& symbol, Attributes&&... attrs);

    /// Append a c-string with given Attributes to the end of the Glyph_string.
    template <typename... Attributes>
    Glyph_string& append(const char* symbols, Attributes&&... attrs);

    /// Append std::string with given Attributes to the end of the Glyph_string.
    template <typename... Attributes>
    Glyph_string& append(const std::string& symbols, Attributes&&... attrs);

    /// Append a wide c-string with given Attributes to the end of Glyph_string.
    template <typename... Attributes>
    Glyph_string& append(const wchar_t* symbols, Attributes&&... attrs);

    /// Append std::wstring with given Attributes to the end of Glyph_string.
    template <typename... Attributes>
    Glyph_string& append(const std::wstring& symbols, Attributes&&... attrs);

    /// Append another Glyph_string with Attributes to the end of Glyph_string.
    template <typename... Attributes>
    Glyph_string& append(const Glyph_string& gs, Attributes&&... attrs);

    /// Add a list of Attributes to every Glyph within the Glyph_string.
    template <typename... Attributes>
    void add_attributes(Attributes&&... attrs);

    /// Remove a single Attribute from every Glyph within the Glyph_string.
    void remove_attribute(Attribute attr);

    // Import member functions from std::vector<Glyph>
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

/// Equality comparison on each Glyph in the Glyph_strings.
bool operator==(const Glyph_string& x, const Glyph_string& y);

/// Inequality comparison on each Glyph in the Glyph_strings.
inline bool operator!=(const Glyph_string& x, const Glyph_string& y) {
    return !(x == y);
}

/// stream output operator for Glyph_string, outputs the Glyphs as chars to os.
inline std::ostream& operator<<(std::ostream& os, const Glyph_string& gs) {
    return os << gs.str();
}

// TEMPLATE IMPLEMENTATIONS
template <typename InputIterator>
Glyph_string::Glyph_string(InputIterator first, InputIterator last)
    : vector<Glyph>::vector(first, last) {}

template <typename... Attributes>
Glyph_string::Glyph_string(const std::string& symbols, Attributes&&... attrs)
    : vector<Glyph>::vector() {
    this->append(symbols, std::forward<Attributes>(attrs)...);
}

template <typename... Attributes>
Glyph_string::Glyph_string(const char* symbols, Attributes&&... attrs) {
    this->append(symbols, std::forward<Attributes>(attrs)...);
}

template <typename... Attributes>
Glyph_string::Glyph_string(wchar_t symbol, Attributes&&... attrs) {
    this->append(Glyph{symbol, std::forward<Attributes>(attrs)...});
}

template <typename... Attributes>
Glyph_string::Glyph_string(const wchar_t* symbols, Attributes&&... attrs) {
    this->append(symbols, std::forward<Attributes>(attrs)...);
}

template <typename... Attributes>
Glyph_string::Glyph_string(const std::wstring& symbols, Attributes&&... attrs)
    : vector<Glyph>::vector() {
    this->append(symbols, std::forward<Attributes>(attrs)...);
}

template <typename... Attributes>
Glyph_string::Glyph_string(const Glyph& glyph, Attributes&&... attrs) {
    this->append(glyph, std::forward<Attributes>(attrs)...);
}

template <typename... Attributes>
Glyph_string::Glyph_string(const std::initializer_list<Glyph>& glyphs,
                           Attributes&&... attrs) {
    this->reserve(glyphs.size());
    for (const Glyph& g : glyphs) {
        this->append(g, std::forward<Attributes>(attrs)...);
    }
}

template <typename... Attributes>
Glyph_string& Glyph_string::append(const Glyph& symbol, Attributes&&... attrs) {
    this->push_back(symbol);
    this->back().brush.add_attributes(std::forward<Attributes>(attrs)...);
    return *this;
}

template <typename... Attributes>
Glyph_string& Glyph_string::append(const char* symbols, Attributes&&... attrs) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring wide_string{converter.from_bytes(symbols)};
    this->reserve(this->size() + wide_string.size());
    for (wchar_t sym : wide_string) {
        this->append(Glyph{sym, std::forward<Attributes>(attrs)...});
    }
    return *this;
}

template <typename... Attributes>
Glyph_string& Glyph_string::append(const std::string& symbols,
                                   Attributes&&... attrs) {
    return this->append(symbols.c_str(), std::forward<Attributes>(attrs)...);
}

template <typename... Attributes>
Glyph_string& Glyph_string::append(const wchar_t* symbols,
                                   Attributes&&... attrs) {
    for (auto i = std::size_t{0}; symbols[i] != L'\0'; ++i) {
        this->append(Glyph{symbols[i], std::forward<Attributes>(attrs)...});
    }
    return *this;
}

template <typename... Attributes>
Glyph_string& Glyph_string::append(const std::wstring& symbols,
                                   Attributes&&... attrs) {
    for (wchar_t sym : symbols) {
        this->append(Glyph{sym, std::forward<Attributes>(attrs)...});
    }
    return *this;
}

template <typename... Attributes>
Glyph_string& Glyph_string::append(const Glyph_string& gs,
                                   Attributes&&... attrs) {
    for (const Glyph& glyph : gs) {
        this->append(glyph, std::forward<Attributes>(attrs)...);
    }
    return *this;
}

template <typename... Attributes>
void Glyph_string::add_attributes(Attributes&&... attrs) {
    for (auto& glyph : *this) {
        glyph.brush.add_attributes(std::forward<Attributes>(attrs)...);
    }
}

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_GLYPH_STRING_HPP
