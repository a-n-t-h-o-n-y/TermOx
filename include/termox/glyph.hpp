#pragma once

#include <concepts>
#include <ranges>
#include <string_view>

#include <esc/brush.hpp>
#include <esc/color.hpp>
#include <esc/trait.hpp>

namespace ox {

using ::esc::Brush;
using ::esc::Color;
using ::esc::ColorIndex;
using ::esc::DefaultColor;
using ::esc::remove_trait;
using ::esc::Trait;
using ::esc::Traits;
using ::esc::TrueColor;

/// A single terminal cell display unit.
/// @details Contains a symbol and a Brush to describe how the symbol should be
/// displayed.
struct Glyph {
    char32_t symbol = U'\0';
    Brush brush     = {};

    [[nodiscard]] constexpr bool operator==(Glyph const&) const = default;
    [[nodiscard]] constexpr bool operator!=(Glyph const&) const = default;
};

// CONCEPTS --------------------------------------------------------------------

/**
 * @brief True if T is a Character type.
 */
template <typename T>
concept Character = std::same_as<T, char> || std::same_as<T, signed char> ||
                    std::same_as<T, unsigned char> ||
                    std::same_as<T, wchar_t> || std::same_as<T, char8_t> ||
                    std::same_as<T, char16_t> || std::same_as<T, char32_t>;

/**
 * @brief True if T is a valid Attribute type.
 */
template <typename T>
concept Attribute =
    std::same_as<T, esc::ColorBG> || std::same_as<T, esc::ColorFG> ||
    std::same_as<T, Trait> || std::same_as<T, esc::RemoveTrait>;

/**
 * @brief True if T is a Forward iterable range of Glyphs.
 */
template <typename T>
concept GlyphString = std::ranges::forward_range<T> &&
                      std::same_as<std::ranges::range_value_t<T>, Glyph>;

// PIPE OPS --------------------------------------------------------------------

// COLORBG ---------------------------------------------------------------------

// Note: For char literal based pipe ops, `using namespace ox` must be in scope.
// ADL issues prevent the pipe ops from being found otherwise.

/**
 * @brief Creates a Glyph with the given symbol and background Color.
 *
 * @param symbol The symbol to display.
 * @param c The ColorBG to apply to the return Glyph's brush.
 * @return A Glyph with the given symbol and Color background.
 */
[[nodiscard]] constexpr auto operator|(Character auto symbol, esc::ColorBG c)
    -> Glyph
{
    return {static_cast<char32_t>(symbol), {.background = c.value}};
}

/**
 * @brief Copy the Glyph and modify its background Color.
 *
 * @param g The Glyph to work from.
 * @param c The ColorBG to apply to the return Glyph's brush.
 * @return The passed in Glyph with the given Color background.
 */
[[nodiscard]] constexpr auto operator|(Glyph g, esc::ColorBG c) -> Glyph
{
    g.brush.background = c.value;
    return g;
}

/**
 * @brief Modify the Glyphs in the GlyphString to have the given background
 * Color.
 *
 * @param gs The GlyphString to work from.
 * @param c The ColorBG to apply to each Glyph's brush.
 * @return The passed in GlyphString with the given Color background.
 */
template <GlyphString T>
constexpr auto operator|(T&& gs, esc::ColorBG c) -> decltype(auto)
{
    for (auto& glyph : gs) {
        glyph.brush.background = c.value;
    }
    return std::forward<T>(gs);
}

// COLORFG ---------------------------------------------------------------------

/**
 * @brief Creates a Glyph with the given symbol and foreground Color.
 *
 * @param symbol The symbol to display.
 * @param c The ColorFG to apply to the return Glyph's brush.
 * @return A Glyph with the given symbol and Color foreground.
 */
[[nodiscard]] constexpr auto operator|(Character auto symbol, esc::ColorFG c)
    -> Glyph
{
    return {static_cast<char32_t>(symbol), {.foreground = c.value}};
}

/**
 * @brief Copy the Glyph and modify its foreground Color.
 *
 * @param g The Glyph to work from.
 * @param c The ColorFG to apply to the return Glyph's brush.
 * @return The passed in Glyph with the given Color foreground.
 */
[[nodiscard]] constexpr auto operator|(Glyph g, esc::ColorFG c) -> Glyph
{
    g.brush.foreground = c.value;
    return g;
}

/**
 * @brief Modify the Glyphs in the GlyphString to have the given foreground
 * Color.
 *
 * @param gs The GlyphString to work from.
 * @param c The ColorFG to apply to each Glyph's brush.
 * @return The passed in GlyphString with the given Color foreground.
 */
template <GlyphString T>
constexpr auto operator|(T&& gs, esc::ColorFG c) -> decltype(auto)
{
    for (auto& glyph : gs) {
        glyph.brush.foreground = c.value;
    }
    return std::forward<T>(gs);
}

// TRAIT -----------------------------------------------------------------------

/**
 * @brief Creates a Glyph with the given symbol and Trait.
 *
 * @param symbol The symbol to display.
 * @param t The Trait to apply to the return Glyph's brush.
 * @return A Glyph with the given symbol and Trait.
 */
[[nodiscard]] constexpr auto operator|(Character auto symbol, Trait t) -> Glyph
{
    return {static_cast<char32_t>(symbol), {.traits = t}};
}

/**
 * @brief Copy the Glyph and add a Trait.
 *
 * @param g The Glyph to work from.
 * @param t The Trait to apply to the return Glyph's brush.
 * @return The passed in Glyph with the given Trait added.
 */
[[nodiscard]] constexpr auto operator|(Glyph g, Trait t) -> Glyph
{
    g.brush.traits = g.brush.traits | t;
    return g;
}

/// Modify the Glyphs in the GlyphString to have the given Trait.
/// @param gs The GlyphString to work from.
/// @param c The Trait to apply to each Glyph's brush.
/// @return The passed in GlyphString with the given Trait.
template <GlyphString T>
constexpr auto operator|(T&& gs, Trait t) -> decltype(auto)
{
    for (auto& glyph : gs) {
        glyph.brush.traits = glyph.brush.traits | t;
    }
    return std::forward<T>(gs);
}

// REMOVE TRAIT ----------------------------------------------------------------

/**
 * @brief Removes a Trait from a Glyph's brush.
 *
 * Use the esc::remove_trait(Trait) function to create the RemoveTrait object.
 *
 * @param g The Glyph to remove the Trait from.
 * @param t The Trait to remove from the Glyph's brush.
 * @return The Glyph with the Trait removed from its brush.
 */
[[nodiscard]] constexpr auto operator|(Glyph g, esc::RemoveTrait t) -> Glyph
{
    g.brush.traits = g.brush.traits | t;
    return g;
}

/**
 * @brief Modify the Glyphs in the GlyphString to remove the given Trait.
 *
 * @param gs The GlyphString to work from.
 * @param c The Trait to remove from each Glyph's brush.
 * @return The passed in GlyphString with the given Trait removed.
 */
template <GlyphString T>
constexpr auto operator|(T&& gs, esc::RemoveTrait t) -> decltype(auto)
{
    for (auto& glyph : gs) {
        glyph.brush.traits = glyph.brush.traits | t;
    }
    return std::forward<T>(gs);
}

// STRINGS ---------------------------------------------------------------------

/**
 * @brief Creates a std::vector<Glyph> from the given u32string_view and
 * Attribute.
 *
 * @param sv The string_view to create Glyphs from.
 * @param attr The Attribute to apply to each Glyph's brush.
 * @return A std::vector<Glyph> with the given Attribute applied to each Glyph.
 */
template <Attribute T>
[[nodiscard]] constexpr auto operator|(std::u32string_view sv, T attr)
    -> std::vector<Glyph>
{
    auto glyphs = std::vector<Glyph>{std::begin(sv), std::end(sv)};
    return glyphs | attr;
}

/**
 * @brief Creates a std::vector<Glyph> from the given u16string_view and
 * Attribute.
 *
 * @param sv The string_view to create Glyphs from.
 * @param attr The Attribute to apply to each Glyph's brush.
 * @return A std::vector<Glyph> with the given Attribute applied to each Glyph.
 */
template <Attribute T>
[[nodiscard]] constexpr auto operator|(std::u16string_view sv, T attr)
    -> std::vector<Glyph>
{
    return {};
    // TODO from here
    // implement this then uncomment line in test file. repeat.
    // TODO review after library is installed
    // auto uStr = icu::UnicodeString{icu::StringPiece{sv.data(), sv.size()}};

    // auto glyphs = std::vector<Glyph>{};
    // glyphs.reserve(uStr.length());

    // for (int32_t i = 0; i < uStr.length(); ++i) {
    //     glyphs.push_back(uStr.char32At(i));
    // }

    // return glyphs | attr;
    // TODO - use ICU to convert to char32_t chars into the vector
    // auto glyphs = std::vector<Glyph>{std::begin(sv), std::end(sv)};
    // return glyphs | attr;
}

/**
 * @brief Creates a std::vector<Glyph> from the given u8string_view and
 * Attribute.
 *
 * @param sv The string_view to create Glyphs from.
 * @param attr The Attribute to apply to each Glyph's brush.
 * @return A std::vector<Glyph> with the given Attribute applied to each
 * Glyph.
 */
template <Attribute T>
[[nodiscard]] constexpr auto operator|(std::u8string_view sv, T attr)
    -> std::vector<Glyph>
{
    return {};
    // TODO - use ICU to convert to char32_t chars into the vector
    // auto glyphs = std::vector<Glyph>{std::begin(sv), std::end(sv)};
    // return glyphs | attr;
}

/**
 * @brief Creates a std::vector<Glyph> from the given string_view and
 * Attribute.
 *
 * @param sv The string_view to create Glyphs from.
 * @param attr The Attribute to apply to each Glyph's brush.
 * @return A std::vector<Glyph> with the given Attribute applied to each
 * Glyph.
 */
template <Attribute T>
[[nodiscard]] constexpr auto operator|(std::string_view sv, T attr)
    -> std::vector<Glyph>
{
    return {};
    // TODO - use ICU to convert to char32_t chars into the vector
    // auto glyphs = std::vector<Glyph>{std::begin(sv), std::end(sv)};
    // return glyphs | attr;
}

}  // namespace ox