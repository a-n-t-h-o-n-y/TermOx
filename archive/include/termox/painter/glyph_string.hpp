#ifndef TERMOX_PAINTER_GLYPH_STRING_HPP
#define TERMOX_PAINTER_GLYPH_STRING_HPP
#include <string>
#include <string_view>
#include <vector>

#include <termox/common/mb_to_u32.hpp>
#include <termox/painter/brush.hpp>
#include <termox/painter/color.hpp>
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

    /// Construct with \p count \p glyph's, adding given Traits to each.
    explicit Glyph_string(Glyph glyph, int count = 1);

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
    auto append(Glyph g) -> Glyph_string&;

    /// Append another Glyph_string to the end of *this.
    auto append(Glyph_string const& gs) -> Glyph_string&;

   public:
    /// Return the number of Glyphs in *this Glyph_string.
    /** Same as size() member function. */
    [[nodiscard]] auto length() const -> int;

    /// Return the number of Glyphs in *this Glyph_string.
    /** Same as length() member function. */
    [[nodiscard]] auto size() const -> int;

   public:
    /// Convert to a std::u32string, each Glyph being a char32_t.
    /** All Brush attributes are lost. */
    [[nodiscard]] auto u32str() const -> std::u32string;

    /// Convert to a std::string.
    /** Each Glyph::symbols is converted to a (potentially) multi-byte char
     *  string. All Brush attributes are lost. */
    [[nodiscard]] auto str() const -> std::string;

   public:
    /// Add \p traits to every Glyph contained in *this.
    void add_traits(Traits traits);

    /// Remove all currently set traits on each Glyph.
    void clear_traits();

    /// Add \p bg as the background color to every Glyph contained in *this.
    void add_color(Background_color bg);

    /// Add \p fg as the foreground color to every Glyph contained in *this.
    void add_color(Foreground_color fg);

    /// Remove a series of Traits from every Glyph within the Glyph_string.
    void remove_traits(Traits traits);

    /// Set the background color as the default for every Glyph in *this.
    void remove_background();

    /// Set the foreground color as the default for every Glyph in *this.
    void remove_foreground();

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

// Traits ----------------------------------------------------------------------

auto operator|=(Glyph_string& gs, Traits ts) -> Glyph_string&;

[[nodiscard]] auto operator|(Glyph_string gs, Traits ts) -> Glyph_string;

}  // namespace ox

namespace esc {  // For ADL; Trait(s) is really in namespace::esc.

[[nodiscard]] auto operator|(char32_t const* gs, Trait t) -> ox::Glyph_string;

[[nodiscard]] auto operator|(char32_t const* gs, Traits ts) -> ox::Glyph_string;

[[nodiscard]] auto operator|(char const* gs, Trait t) -> ox::Glyph_string;

[[nodiscard]] auto operator|(char const* gs, Traits ts) -> ox::Glyph_string;

}  // namespace esc

namespace ox {

// Background_color ------------------------------------------------------------
auto operator|=(Glyph_string& gs, Background_color c) -> Glyph_string&;

[[nodiscard]] auto operator|(Glyph_string gs, Background_color c)
    -> Glyph_string;

[[nodiscard]] auto operator|(char32_t const* gs, Background_color c)
    -> Glyph_string;

// Foreground_color ------------------------------------------------------------
auto operator|=(Glyph_string& gs, Foreground_color c) -> Glyph_string&;

[[nodiscard]] auto operator|(Glyph_string gs, Foreground_color c)
    -> Glyph_string;

[[nodiscard]] auto operator|(char32_t const* gs, Foreground_color c)
    -> Glyph_string;

// Brush -----------------------------------------------------------------------
/// Assigns the Brush \p b to each Glyph in \p gs.
auto operator|=(Glyph_string& gs, Brush b) -> Glyph_string&;

[[nodiscard]] auto operator|(Glyph_string gs, Brush b) -> Glyph_string;

/// Assigns the Brush \p b to each Glyph in \p gs.
[[nodiscard]] auto operator|(char32_t const* gs, Brush b) -> Glyph_string;

// Comparison ------------------------------------------------------------------

/// Equality comparison on each Glyph in the Glyph_strings.
[[nodiscard]] auto operator==(Glyph_string const& x, Glyph_string const& y)
    -> bool;

/// Inequality comparison on each Glyph in the Glyph_strings.
[[nodiscard]] auto operator!=(Glyph_string const& x, Glyph_string const& y)
    -> bool;

}  // namespace ox
#endif  // TERMOX_PAINTER_GLYPH_STRING_HPP
