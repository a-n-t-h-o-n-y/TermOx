#ifndef TERMOX_WIDGET_WIDGETS_TILE_HPP
#define TERMOX_WIDGET_WIDGETS_TILE_HPP
#include <termox/painter/glyph.hpp>
#include <termox/painter/painter.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

/// A unit width/height Widget that can display a single Glyph.
class Tile : public Widget {
   public:
    struct Parameters {
        Glyph tile = U' ';
    };

   public:
    explicit Tile(Glyph tile = U' ') : tile_{tile}
    {
        *this | pipe::fixed_width(1) | pipe::fixed_height(1);
    }

    explicit Tile(Parameters parameters) : Tile{std::move(parameters.tile)} {}

   public:
    void set_tile(Glyph tile)
    {
        tile_ = tile;
        this->update();
    }

    [[nodiscard]] auto tile() -> Glyph { return tile_; }

   protected:
    auto paint_event(Painter& p) -> bool override
    {
        p.put(tile_, {0, 0});
        return Widget::paint_event(p);
    }

   private:
    Glyph tile_;
};

/// Helper function to create a Tile instance.
[[nodiscard]] inline auto tile(Glyph tile = U' ') -> std::unique_ptr<Tile>
{
    return std::make_unique<Tile>(std::move(tile));
}

/// Helper function to create a Tile instance.
[[nodiscard]] inline auto tile(Tile::Parameters parameters)
    -> std::unique_ptr<Tile>
{
    return std::make_unique<Tile>(std::move(parameters));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_TILE_HPP
