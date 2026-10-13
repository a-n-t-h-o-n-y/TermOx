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

inline auto operator""_gs(char32_t const* str, std::size_t count) -> std::vector<Glyph>
{ return esc::glyphs(std::u32string_view{str, count}); }

inline auto operator""_gs(char const* str, std::size_t count) -> std::vector<Glyph>
{ return esc::glyphs(std::string_view{str, count}); }

}  // namespace ox