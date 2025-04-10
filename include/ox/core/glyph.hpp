#pragma once

#include <cstddef>
#include <string_view>
#include <vector>

#include <esc/brush.hpp>
#include <esc/color.hpp>
#include <esc/glyph.hpp>
#include <esc/trait.hpp>

namespace ox {

using ::esc::bg;
using ::esc::Brush;
using ::esc::Character;
using ::esc::Color;
using ::esc::ColorBG;
using ::esc::ColorFG;
using ::esc::fg;
using ::esc::Glyph;
using ::esc::GlyphString;
using ::esc::HSL;
using ::esc::InsertableGlyphString;
using ::esc::RGB;
using ::esc::TColor;
using ::esc::TermColor;
using ::esc::Trait;
using ::esc::Traits;
using ::esc::TrueColor;
using ::esc::XColor;

inline auto operator""_gs(char32_t const* str, std::size_t count) -> std::vector<Glyph>
{
    return esc::detail::utf32_to_glyphs(std::u32string_view{str, count});
}

inline auto operator""_gs(char const* str, std::size_t count) -> std::vector<Glyph>
{
    return esc::detail::utf8_to_glyphs(std::string_view{str, count});
}

}  // namespace ox