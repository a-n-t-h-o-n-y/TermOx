#ifndef CPPURSES_PAINTER_DETAIL_ADD_DEFAULT_ATTRIBUTES_HPP
#define CPPURSES_PAINTER_DETAIL_ADD_DEFAULT_ATTRIBUTES_HPP

namespace cppurses {
struct Glyph;
class Brush;
namespace detail {

void add_default_attributes(Glyph& tile, const Brush& brush);

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_ADD_DEFAULT_ATTRIBUTES_HPP
