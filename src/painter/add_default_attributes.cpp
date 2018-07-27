#include <cppurses/painter/detail/add_default_attributes.hpp>

#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/glyph.hpp>

namespace cppurses {
namespace detail {

void add_default_attributes(Glyph& tile, const Brush& brush) {
    if (!tile.brush.background_color() && brush.background_color()) {
        tile.brush.add_attributes(background(*brush.background_color()));
    }
    if (!tile.brush.foreground_color() && brush.foreground_color()) {
        tile.brush.add_attributes(foreground(*brush.foreground_color()));
    }
    for (const auto& attr : brush.attributes()) {
        tile.brush.add_attributes(attr);
    }
}

}  // namespace detail
}  // namespace cppurses
