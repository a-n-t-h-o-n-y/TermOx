#pragma once

#include <concepts>
#include <ranges>
#include <string_view>

#include <esc/brush.hpp>
#include <esc/color.hpp>
#include <esc/glyph.hpp>
#include <esc/trait.hpp>

namespace ox {

using ::esc::Brush;
using ::esc::Color;
using ::esc::ColorIndex;
using ::esc::DefaultColor;
using ::esc::Glyph;
using ::esc::GlyphString;
using ::esc::remove_trait;
using ::esc::Trait;
using ::esc::Traits;
using ::esc::TrueColor;

}  // namespace ox