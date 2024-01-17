#ifndef TERMOX_WIDGET_WIDGETS_TILE_HPP
#define TERMOX_WIDGET_WIDGETS_TILE_HPP
#include <memory>

#include <termox/painter/glyph.hpp>
#include <termox/painter/painter.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

/// A unit width/height Widget that can display a single Glyph.
class Tile : public Widget {
   public:
    struct Parameters {
        Glyph tile = U' ';
    };

   public:
    explicit Tile(Glyph tile = U' ');

    explicit Tile(Parameters p);

   public:
    void set_tile(Glyph tile);

    [[nodiscard]] auto tile() -> Glyph;

   protected:
    auto paint_event(Painter& p) -> bool override;

   private:
    Glyph tile_;
};

/// Helper function to create a Tile instance.
[[nodiscard]] auto tile(Glyph tile = U' ') -> std::unique_ptr<Tile>;

/// Helper function to create a Tile instance.
[[nodiscard]] auto tile(Tile::Parameters parameters) -> std::unique_ptr<Tile>;

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_TILE_HPP
