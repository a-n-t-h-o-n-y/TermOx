#include <cppurses/painter/detail/add_default_attributes.hpp>

#include <array>

#include <optional/optional.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
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
    for (Attribute a : Attribute_list) {
        if (brush.has_attribute(a)) {
            tile.brush.add_attributes(a);
        }
    }
}

}  // namespace detail
}  // namespace cppurses
