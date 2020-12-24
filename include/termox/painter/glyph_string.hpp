#ifndef TERMOX_PAINTER_GLYPH_STRING_HPP
#define TERMOX_PAINTER_GLYPH_STRING_HPP
#include <algorithm>
#include <codecvt>
#include <cstddef>
#include <locale>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include <termox/painter/glyph.hpp>
#include <termox/painter/trait.hpp>
#include <termox/painter/utility/wchar_to_bytes.hpp>

namespace ox {

/// Holds a collection of Glyphs with a similar interface to std::string.
class Glyph_string : private std::vector<Glyph> {
   public:
    /// Used to indicate 'Until the end of the string'.
    static auto constexpr npos = static_cast<std::size_t>(-1);

   public:
    /// Default constructs an empty Glyph_string.
    Glyph_string() = default;

    Glyph_string(Glyph_string const&) = default;
    Glyph_string(Glyph_string&&)      = default;
    auto operator=(Glyph_string const&) -> Glyph_string& = default;
    auto operator=(Glyph_string&&) -> Glyph_string& = default;

    /// Construct with \p symbols, each having given Traits applied to them.
    template <typename... Traits>
    Glyph_string(std::wstring const& symbols, Traits... traits)
    {
        this->append(symbols, traits...);
    }

    /// Construct with \p symbols, each having given Traits applied to them.
    template <typename... Traits>
    Glyph_string(wchar_t const* symbols, Traits... traits)
    {
        this->append(symbols, traits...);
    }

    // TODO make this explicit, to encourage wchar_t strings so no conversions
    /// Construct with \p symbols, each having given Traits applied to them.
    template <typename... Traits>
    Glyph_string(std::string const& symbols, Traits... traits)
    {
        this->append(symbols, traits...);
    }

    // TODO make this explicit, to encourage wchar_t strings so no conversions
    /// Construct with \p symbols, each having given Traits applied to them.
    template <typename... Traits>
    Glyph_string(char const* symbols, Traits... traits)
    {
        this->append(symbols, traits...);
    }

    /// Construct with \p count \p glyph's, adding given Traits to each.
    explicit Glyph_string(Glyph glyph, std::size_t count = 1)
    {
        for (; count != 0; --count)
            this->append(glyph);
    }

    /// Construct with iterators from any container providing Input Iterators.
    template <typename InputIterator>
    Glyph_string(InputIterator first, InputIterator last)
        : vector<Glyph>::vector(first, last)
    {}

   public:
    /// Convert to a std::wstring, each Glyph being a wchar_t.
    auto w_str() const -> std::wstring
    {
        auto result = std::wstring{};
        result.reserve(this->length());
        for (auto const& glyph : *this)
            result.push_back(glyph.symbol);
        return result;
    }

    /// Convert to a std::string.
    auto str() const -> std::string
    {
        return utility::wchar_to_bytes(this->w_str());
    }

    /// Return the length in Glyphs of the Glyph_string.
    auto length() const -> size_type { return this->size(); }

   public:
    /// Compound concatenation assignment operator to append a Glyph.
    auto operator+=(Glyph glyph) -> Glyph_string&
    {
        return this->append(glyph);
    }

    /// Concatenation operator to append a Glyph_string.
    auto operator+(Glyph g) const -> Glyph_string
    {
        return Glyph_string{*this}.append(g);
    }

    /// Concatenation operator to append a Glyph_string.
    friend auto operator+(Glyph lhs, Glyph_string const& rhs) -> Glyph_string
    {
        return Glyph_string{lhs}.append(rhs);
    }

    /// Concatenation operator to append a Glyph_string.
    auto operator+(Glyph_string const& gs) const -> Glyph_string
    {
        return Glyph_string{*this}.append(gs);
    }

    /// Concatenation operator to append a Glyph_string.
    friend auto operator+(wchar_t const* lhs, Glyph_string const& rhs)
        -> Glyph_string
    {
        return Glyph_string{lhs}.append(rhs);
    }

    /// Concatenation operator to append a Glyph_string.
    friend auto operator+(std::wstring const& lhs, Glyph_string const& rhs)
        -> Glyph_string
    {
        return Glyph_string{lhs}.append(rhs);
    }

    /// Append single Glyph to the end of the Glyph_string w/given Traits.
    template <typename... Traits>
    auto append(Glyph symbol, Traits... traits) -> Glyph_string&
    {
        this->push_back(symbol);
        this->back().brush.add_traits(traits...);
        return *this;
    }

    /// Append a c-string with given Traits to the end of the Glyph_string.
    template <typename... Traits>
    auto append(char const* symbols, Traits... traits) -> Glyph_string&
    {
        auto converter   = std::wstring_convert<std::codecvt_utf8<wchar_t>>{};
        auto wide_string = std::wstring{converter.from_bytes(symbols)};
        this->reserve(this->size() + wide_string.size());
        for (wchar_t sym : wide_string)
            this->append(Glyph{sym, traits...});
        return *this;
    }

    /// Append std::string with given Traits to the end of the Glyph_string.
    template <typename... Traits>
    auto append(std::string const& symbols, Traits... traits) -> Glyph_string&
    {
        return this->append(symbols.c_str(), traits...);
    }

    /// Append a wide c-string with given Traits to the end of Glyph_string.
    template <typename... Traits>
    auto append(wchar_t const* symbols, Traits... traits) -> Glyph_string&
    {
        for (auto i = 0uL; symbols[i] != L'\0'; ++i)
            this->append(Glyph{symbols[i], traits...});
        return *this;
    }

    /// Append std::wstring with given Traits to the end of Glyph_string.
    template <typename... Traits>
    auto append(std::wstring const& symbols, Traits... traits) -> Glyph_string&
    {
        for (wchar_t sym : symbols)
            this->append(Glyph{sym, traits...});
        return *this;
    }

    /// Append another Glyph_string with Traits to the end of Glyph_string.
    template <typename... Traits>
    auto append(Glyph_string const& gs, Traits... traits) -> Glyph_string&
    {
        for (Glyph g : gs)
            this->append(g, traits...);
        return *this;
    }

    /// Add a list of Traits to every Glyph within the Glyph_string.
    template <typename... Traits>
    void add_traits(Traits... traits)
    {
        for (auto& glyph : *this)
            glyph.brush.add_traits(traits...);
    }

    /// Remove a series of Traits from every Glyph within the Glyph_string.
    template <typename... Traits>
    void remove_traits(Traits... x)
    {
        for (Glyph& glyph : *this)
            glyph.brush.remove_traits(x...);
    }

   public:
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

// Trait -----------------------------------------------------------------------
inline auto operator|(Glyph_string& gs, Trait t) -> Glyph_string&
{
    gs.add_traits(t);
    return gs;
}

inline auto operator|(Glyph_string&& gs, Trait t) -> Glyph_string
{
    return gs | t;
}

inline auto operator|(wchar_t const* gs, Trait t) -> Glyph_string
{
    return Glyph_string{gs} | t;
}

// Background_color ------------------------------------------------------------
inline auto operator|(Glyph_string& gs, Background_color c) -> Glyph_string&
{
    gs.add_traits(c);
    return gs;
}

inline auto operator|(Glyph_string&& gs, Background_color c) -> Glyph_string
{
    return gs | c;
}

inline auto operator|(wchar_t const* gs, Background_color c) -> Glyph_string
{
    return Glyph_string{gs} | c;
}

// Foreground_color ------------------------------------------------------------
inline auto operator|(Glyph_string& gs, Foreground_color c) -> Glyph_string&
{
    gs.add_traits(c);
    return gs;
}

inline auto operator|(Glyph_string&& gs, Foreground_color c) -> Glyph_string
{
    return gs | c;  // TODO doesn't this just call the l value version?
}

inline auto operator|(wchar_t const* gs, Foreground_color c) -> Glyph_string
{
    return Glyph_string{gs} | c;
}

// Brush ------------------------------------------------------------
/// Assigns the Brush \p b to each Glyph in \p gs.
inline auto operator|(Glyph_string& gs, Brush b) -> Glyph_string&
{
    for (auto& g : gs)
        g.brush = b;
    return gs;
}

/// Assigns the Brush \p b to each Glyph in \p gs.
inline auto operator|(Glyph_string&& gs, Brush b) -> Glyph_string
{
    return gs | b;
}

/// Assigns the Brush \p b to each Glyph in \p gs.
inline auto operator|(wchar_t const* gs, Brush b) -> Glyph_string
{
    return Glyph_string{gs} | b;
}

/// Equality comparison on each Glyph in the Glyph_strings.
inline auto operator==(Glyph_string const& x, Glyph_string const& y) -> bool
{
    return std::equal(std::begin(x), std::end(x), std::begin(y), std::end(y));
}

/// Inequality comparison on each Glyph in the Glyph_strings.
inline auto operator!=(Glyph_string const& x, Glyph_string const& y) -> bool
{
    return not(x == y);
}

}  // namespace ox
#endif  // TERMOX_PAINTER_GLYPH_STRING_HPP
