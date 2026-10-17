/** @file
 * Escape colors, styles, glyphs, and UTF-8/UTF-32 glyph literals.
 *
 * TermOx re-exports the drawing types from Escape in namespace ox:
 * - `Color` holds `default_color`, `IndexedColor` (terminal palette slot 0..255),
 *   or `RGB` (three bytes, or a packed `0xRRGGBB` constructor).
 * - `colors` contains named terminal palette slots; their appearance depends on
 *   the terminal. `fg(color)` and `bg(color)` create foreground/background patches.
 * - `Style` replaces foreground, background, and traits together. `StylePatch`
 *   changes only present color channels, adds enabled traits, and removes disabled
 *   traits. An absent color preserves it; `default_color` resets that channel.
 * - `Glyph` holds a Unicode scalar in `symbol` and a `Style`; it does not imply
 *   one terminal column. `GlyphString` is `std::vector<Glyph>`. Frame cells own
 *   complete graphemes instead.
 *
 * The upstream declarations and operators are in
 * [Escape's public headers](https://github.com/a-n-t-h-o-n-y/Escape/tree/c714abfa0061526ec0d9ab593b4a8ab447c1d412/include/esc).
 * @see Frame, Canvas, put()
 */
#pragma once

#include <cstddef>
#include <string_view>
#include <vector>

#include <esc/color.hpp>
#include <esc/glyph.hpp>
#include <esc/style.hpp>
#include <esc/trait.hpp>

namespace ox {

using ::esc::bg;
using ::esc::Character;
using ::esc::Color;
using ::esc::ColorBG;
using ::esc::ColorFG;
using ::esc::default_color;
using ::esc::fg;
using ::esc::Glyph;
using ::esc::GlyphRange;
using ::esc::GlyphString;
using ::esc::HSL;
using ::esc::IndexedColor;
using ::esc::RGB;
using ::esc::Style;
using ::esc::StylePatch;
using ::esc::Trait;
using ::esc::Traits;
namespace colors = ::esc::colors;

/** Reject styles with a valueless color channel before drawing. */
void validate(Style const& style);

/** Convert a UTF-32 literal into styled glyph storage using Escape's Unicode conversion. */
inline auto operator""_gs(char32_t const* str, std::size_t count) -> std::vector<Glyph>
{ return esc::glyphs(std::u32string_view{str, count}); }

/** Convert a UTF-8 literal into owned styled glyph storage; malformed UTF-8 is rejected. */
inline auto operator""_gs(char const* str, std::size_t count) -> std::vector<Glyph>
{ return esc::glyphs(std::string_view{str, count}); }

}  // namespace ox