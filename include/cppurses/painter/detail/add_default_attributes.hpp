#ifndef CPPURSES_PAINTER_DETAIL_ADD_DEFAULT_ATTRIBUTES_HPP
#define CPPURSES_PAINTER_DETAIL_ADD_DEFAULT_ATTRIBUTES_HPP

namespace cppurses {
struct Glyph;
class Widget;
namespace detail {

void add_default_attributes(Glyph& tile, Widget* w) {
    if (!tile.brush.background_color() && w->brush.background_color()) {
        tile.brush.add_attributes(background(*w->brush.background_color()));
    }
    if (!tile.brush.foreground_color() && w->brush.foreground_color()) {
        tile.brush.add_attributes(foreground(*w->brush.foreground_color()));
    }
    for (const auto& attr : w->brush.attributes()) {
        tile.brush.add_attributes(attr);
    }
}

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_ADD_DEFAULT_ATTRIBUTES_HPP
