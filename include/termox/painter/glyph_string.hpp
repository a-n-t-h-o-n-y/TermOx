#ifndef TERMOX_PAINTER_GLYPH_STRING_HPP
#define TERMOX_PAINTER_GLYPH_STRING_HPP
#include <algorithm>
#include <codecvt>
#include <cstddef>
#include <locale>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <termox/common/mb_to_u32.hpp>
#include <termox/common/overload.hpp>
#include <termox/common/u32_to_mb.hpp>
#include <termox/painter/brush.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/painter/trait.hpp>

namespace ox {

// TODO hold a vector instead of inherit from one, explicitly defer to vector
// methods instead of using = statements.

/// Holds a collection of Glyphs with a similar interface to std::string.
class Glyph_string : private std::vector<Glyph> {
   public:
    /// Used to indicate 'Until the end of the string'.
    static constexpr auto npos = -1;

   public:
    /// Default constructs an empty Glyph_string.
    Glyph_string() = default;

    Glyph_string(Glyph_string const&) = default;

    Glyph_string(Glyph_string&&) noexcept(
        std::is_nothrow_move_constructible_v<std::vector<Glyph>>) = default;

    auto operator=(Glyph_string const&) -> Glyph_string& = default;

    auto operator=(Glyph_string&&) noexcept(
        std::is_nothrow_move_constructible_v<std::vector<Glyph>>)
        -> Glyph_string& = default;

    /// Construct with \p count \p glyph's, adding given Traits to each.
    explicit Glyph_string(Glyph glyph, int count = 1)
    {
        for (; count != 0; --count)
            this->append(glyph);
    }

    /// Construct with a Glyph for each char32_t in \p symbols and Brush \p b.
    /** Attributes can be background/foreground Colors and Traits. */
    template <typename... Attributes>
    Glyph_string(std::u32string_view symbols, Attributes... attrs)
    {
        for (char32_t c : symbols)
            this->append(Glyph{c, Brush{attrs...}});
    }
    template <typename... Attributes>
    Glyph_string(char32_t const* symbols, Attributes... attrs)
        : Glyph_string{std::u32string_view{symbols}, attrs...}
    {}
    template <typename... Attributes>
    Glyph_string(std::u32string const& symbols, Attributes... attrs)
        : Glyph_string{std::u32string_view{symbols}, attrs...}
    {}

    /// Construct with a Glyph for each character in \p symbols and Brush \p b.
    /** This allows for mult-byte characters in \p symbols, using the currently
     *  set clocale for the conversion. This is set to utf8 by this library.
     *  Attributes can be background/foreground Colors and Traits. */
    template <typename... Attributes>
    Glyph_string(std::string_view symbols, Attributes... attrs)
        : Glyph_string{mb_to_u32(symbols), attrs...}
    {}
    template <typename... Attributes>
    Glyph_string(char const* symbols, Attributes... attrs)
        : Glyph_string{std::string_view{symbols}, attrs...}
    {}
    template <typename... Attributes>
    Glyph_string(std::string const& symbols, Attributes... attrs)
        : Glyph_string{std::string_view{symbols}, attrs...}
    {}

    /// Construct with iterators from any container providing Input Iterators.
    /** The iterator type must be iterating over Glyphs. */
    template <typename InputIterator>
    Glyph_string(InputIterator first, InputIterator last)
        : vector<Glyph>::vector(first, last)
    {}

   public:
    /// Append a single Glyph to the end of *this.
    auto append(Glyph g) -> Glyph_string&
    {
        this->push_back(g);
        return *this;
    }

    /// Append another Glyph_string to the end of *this.
    auto append(Glyph_string const& gs) -> Glyph_string&
    {
        for (Glyph g : gs)
            this->append(g);
        return *this;
    }

   public:
    /// Return the number of Glyphs in *this Glyph_string.
    /** Same as size() member function. */
    [[nodiscard]] auto length() const -> int { return this->size(); }

    /// Return the number of Glyphs in *this Glyph_string.
    /** Same as length() member function. */
    [[nodiscard]] auto size() const -> int { return this->vector::size(); }

   public:
    /// Convert to a std::u32string, each Glyph being a char32_t.
    /** All Brush attributes are lost. */
    [[nodiscard]] auto u32str() const -> std::u32string
    {
        auto result = std::u32string{};
        result.reserve(this->size());
        for (Glyph g : *this)
            result.push_back(g.symbol);
        return result;
    }

    /// Convert to a std::string.
    /** Each Glyph::symbols is converted to a (potentially) multi-byte char
     *  string. All Brush attributes are lost. */
    [[nodiscard]] auto str() const -> std::string
    {
        return u32_to_mb(this->u32str());
    }

   public:
    /// Add \p traits to every Glyph contained in *this.
    void add_traits(Traits traits)
    {
        for (auto& glyph : *this)
            glyph.brush.traits.insert(traits);
    }

    /// Remove all currently set traits on each Glyph.
    void clear_traits()
    {
        for (auto& glyph : *this)
            glyph.brush.traits = Trait::None;
    }

    /// Add \p bg as the background color to every Glyph contained in *this.
    void add_color(Background_color bg)
    {
        for (auto& glyph : *this)
            glyph.brush.background = Color{bg.value};
    }

    /// Add \p fg as the foreground color to every Glyph contained in *this.
    void add_color(Foreground_color fg)
    {
        for (auto& glyph : *this)
            glyph.brush.foreground = Color{fg.value};
    }

    /// Remove a series of Traits from every Glyph within the Glyph_string.
    void remove_traits(Traits traits)
    {
        for (Glyph& glyph : *this)
            glyph.brush.traits.remove(traits);
    }

    /// Set the background color as the default for every Glyph in *this.
    void remove_background()
    {
        for (auto& glyph : *this)
            glyph.brush.background = Color::Background;
    }

    /// Set the foreground color as the default for every Glyph in *this.
    void remove_foreground()
    {
        for (auto& glyph : *this)
            glyph.brush.foreground = Color::Foreground;
    }

   public:
    using size_type = int;
    using std::vector<Glyph>::value_type;
    using std::vector<Glyph>::allocator_type;
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
/// Modifying Operation
inline auto operator|(Glyph_string& gs, Traits ts) -> Glyph_string&
{
    gs.add_traits(ts);
    return gs;
}

[[nodiscard]] inline auto operator|(Glyph_string const& gs, Traits ts)
    -> Glyph_string
{
    auto copy = gs;
    copy.add_traits(ts);
    return copy;
}

[[nodiscard]] inline auto operator|(Glyph_string&& gs, Traits ts)
    -> Glyph_string
{
    return gs | ts;
}

}  // namespace ox

namespace esc {  // For ADL; Trait(s) is really in namespace::esc.
[[nodiscard]] inline auto operator|(char32_t const* gs, Trait t)
    -> ox::Glyph_string
{
    return ox::Glyph_string{gs} | t;
}

[[nodiscard]] inline auto operator|(char32_t const* gs, Traits ts)
    -> ox::Glyph_string
{
    return ox::Glyph_string{gs} | ts;
}

[[nodiscard]] inline auto operator|(char const* gs, Trait t) -> ox::Glyph_string
{
    return ox::Glyph_string{gs} | t;
}

[[nodiscard]] inline auto operator|(char const* gs, Traits ts)
    -> ox::Glyph_string
{
    return ox::Glyph_string{gs} | ts;
}
}  // namespace esc

namespace ox {

// Background_color ------------------------------------------------------------
inline auto operator|(Glyph_string& gs, Background_color c) -> Glyph_string&
{
    gs.add_color(c);
    return gs;
}

[[nodiscard]] inline auto operator|(Glyph_string const& gs, Background_color c)
    -> Glyph_string
{
    auto copy = gs;
    copy.add_color(c);
    return copy;
}

[[nodiscard]] inline auto operator|(Glyph_string&& gs, Background_color c)
    -> Glyph_string
{
    return gs | c;
}

[[nodiscard]] inline auto operator|(char32_t const* gs, Background_color c)
    -> Glyph_string
{
    return Glyph_string{gs} | c;
}

// Foreground_color ------------------------------------------------------------
inline auto operator|(Glyph_string& gs, Foreground_color c) -> Glyph_string&
{
    gs.add_color(c);
    return gs;
}

[[nodiscard]] inline auto operator|(Glyph_string const& gs, Foreground_color c)
    -> Glyph_string
{
    auto copy = gs;
    copy.add_color(c);
    return copy;
}

[[nodiscard]] inline auto operator|(Glyph_string&& gs, Foreground_color c)
    -> Glyph_string
{
    return gs | c;
}

[[nodiscard]] inline auto operator|(char32_t const* gs, Foreground_color c)
    -> Glyph_string
{
    return Glyph_string{gs} | c;
}

// Brush -----------------------------------------------------------------------
/// Assigns the Brush \p b to each Glyph in \p gs.
inline auto operator|(Glyph_string& gs, Brush b) -> Glyph_string&
{
    for (auto& g : gs)
        g.brush = b;
    return gs;
}

[[nodiscard]] inline auto operator|(Glyph_string const& gs, Brush b)
    -> Glyph_string
{
    auto copy = gs;
    for (auto& g : copy)
        g.brush = b;
    return copy;
}

/// Assigns the Brush \p b to each Glyph in \p gs.
[[nodiscard]] inline auto operator|(Glyph_string&& gs, Brush b) -> Glyph_string
{
    return gs | b;
}

/// Assigns the Brush \p b to each Glyph in \p gs.
[[nodiscard]] inline auto operator|(char32_t const* gs, Brush b) -> Glyph_string
{
    return Glyph_string{gs} | b;
}

// Comparison ------------------------------------------------------------------

/// Equality comparison on each Glyph in the Glyph_strings.
[[nodiscard]] inline auto operator==(Glyph_string const& x,
                                     Glyph_string const& y) -> bool
{
    return std::equal(std::begin(x), std::end(x), std::begin(y), std::end(y));
}

/// Inequality comparison on each Glyph in the Glyph_strings.
[[nodiscard]] inline auto operator!=(Glyph_string const& x,
                                     Glyph_string const& y) -> bool
{
    return !(x == y);
}

}  // namespace ox
#endif  // TERMOX_PAINTER_GLYPH_STRING_HPP
