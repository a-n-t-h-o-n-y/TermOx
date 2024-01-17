#include <termox/widget/widgets/tile.hpp>

#include <memory>
#include <utility>

#include <termox/painter/glyph.hpp>
#include <termox/painter/painter.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

Tile::Tile(Glyph tile) : tile_{tile}
{
    *this | pipe::fixed_width(1) | pipe::fixed_height(1);
}

Tile::Tile(Parameters p) : Tile{std::move(p.tile)} {}

void Tile::set_tile(Glyph tile)
{
    tile_ = tile;
    this->update();
}

auto Tile::tile() -> Glyph { return tile_; }

auto Tile::paint_event(Painter& p) -> bool
{
    p.put(tile_, {0, 0});
    return Widget::paint_event(p);
}

auto tile(Glyph tile) -> std::unique_ptr<Tile>
{
    return std::make_unique<Tile>(std::move(tile));
}

auto tile(Tile::Parameters p) -> std::unique_ptr<Tile>
{
    return std::make_unique<Tile>(std::move(p));
}

}  // namespace ox
