#ifndef CPPURSES_PAINTER_DETAIL_GLYPH_AND_BKGD_BOOL_HPP
#define CPPURSES_PAINTER_DETAIL_GLYPH_AND_BKGD_BOOL_HPP
#include <cppurses/painter/glyph.hpp>

namespace cppurses {
namespace detail {

/// Stores a Glyph and a bool to determine if the Glyph is a background tile
/// or not. Used within Painter and Paint_buffer.
struct Glyph_and_bkgd_bool {
    Glyph tile;
    bool is_background;
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_GLYPH_AND_BKGD_BOOL_HPP
