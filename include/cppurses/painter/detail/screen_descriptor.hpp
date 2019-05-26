#ifndef CPPURSES_PAINTER_DETAIL_SCREEN_DESCRIPTOR_HPP
#define CPPURSES_PAINTER_DETAIL_SCREEN_DESCRIPTOR_HPP
#include <unordered_map>

#include <cppurses/painter/glyph.hpp>
#include <cppurses/widget/point.hpp>

namespace cppurses {
namespace detail {

/// Holds the screen state by Points on the screen and corresponding Glyphs.
using Screen_descriptor = std::unordered_map<Point, Glyph>;

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_SCREEN_DESCRIPTOR_HPP
