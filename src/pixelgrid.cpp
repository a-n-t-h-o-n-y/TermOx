#include <ox/pixelgrid.hpp>

#include <cstddef>
#include <memory>

namespace ox {

auto PixelGrid::Matrix::operator[](Point p) -> TrueColor&
{
    return pixels[(std::size_t)(p.x + p.y * area.width)];
}

auto PixelGrid::Matrix::operator[](Point p) const -> TrueColor
{
    return pixels[(std::size_t)(p.x + p.y * area.width)];
}

PixelGrid::PixelGrid(Area area)
    : Widget{FocusPolicy::None, SizePolicy::flex()},
      buffer{
          .area = area,
          .pixels = {(std::size_t)(area.width * area.height), TrueColor{0x000000}},
      }
{}

PixelGrid::PixelGrid(Matrix matrix)
    : Widget{FocusPolicy::None, SizePolicy::flex()}, buffer{std::move(matrix)}
{}

void PixelGrid::paint(Canvas c)
{
    for (auto y = 0; y < buffer.area.height; y += 2) {
        for (auto x = 0; x < buffer.area.width; ++x) {
            auto const color_top = buffer[{x, y}];
            auto const color_bottom =
                (y + 1 < buffer.area.height) ? buffer[{x, y + 1}] : TrueColor{0x000000};
            auto const at = Point{x, y / 2};
            if (at.x < c.size.width && at.y < c.size.height) {
                c[at] = U'▄' | bg(color_top) | fg(color_bottom);
            }
        }
    }
}

}  // namespace ox