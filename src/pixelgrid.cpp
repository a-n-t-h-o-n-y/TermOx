#include <ox/pixelgrid.hpp>

#include <limits>
#include <stdexcept>
#include <utility>

namespace ox {
auto pixel_count(Area size) -> std::size_t
{
    validate(size);
    auto const width = (std::size_t)size.width;
    auto const height = (std::size_t)size.height;
    if (height != 0 && width > std::numeric_limits<std::size_t>::max() / height) {
        throw std::length_error{"Pixel dimensions overflow storage count"};
    }
    auto const count = width * height;
    if (count > (std::size_t)std::numeric_limits<std::ptrdiff_t>::max() / sizeof(RGB)) {
        throw std::length_error{"Pixel storage exceeds addressable byte range"};
    }
    return count;
}
auto pixel_index(Area size, Point at) -> std::size_t
{
    (void)pixel_count(size);
    if (!contains({{}, size}, at)) { throw std::out_of_range{"Pixel coordinate"}; }
    return (std::size_t)at.y * (std::size_t)size.width + (std::size_t)at.x;
}
auto pixel_cell_size(Area size) -> Area
{
    validate(size);
    return {size.width, size.height / 2 + size.height % 2};
}
void paint_pixels(Canvas canvas,
                  Area size,
                  std::span<RGB const> pixels,
                  Color odd_pixel)
{
    if (pixels.size() != pixel_count(size)) {
        throw std::invalid_argument{"Pixel storage does not match dimensions"};
    }
    auto const clip = intersection(canvas.visible(), {{}, pixel_cell_size(size)});
    if (clip.size.width == 0 || clip.size.height == 0) { return; }
    if (esc::grapheme_width(U"▄", canvas.width_policy()) !=
        std::optional<std::size_t>{1}) {
        throw std::invalid_argument{
            "Pixel drawing requires a one-column lower-half block"};
    }
    auto const width = (std::size_t)size.width;
    for (auto y = clip.at.y; y < clip.at.y + clip.size.height; ++y) {
        auto const row = (std::size_t)y * 2;
        auto index = row * width + (std::size_t)clip.at.x;
        auto const bottom = row + 1 < (std::size_t)size.height;
        for (auto x = clip.at.x; x < clip.at.x + clip.size.width; ++x, ++index) {
            canvas.write(
                {x, y}, U"▄",
                {.background = pixels[index],
                 .foreground = bottom ? Color{pixels[index + width]} : odd_pixel});
        }
    }
}
PixelGrid::PixelGrid(Area size, RGB fill)
    : Widget{FocusPolicy::None, SizePolicy::flex()},
      size_{size},
      pixels_(pixel_count(size), fill)
{}
PixelGrid::PixelGrid(Area size, std::vector<RGB> pixels)
    : Widget{FocusPolicy::None, SizePolicy::flex()},
      size_{size},
      pixels_{std::move(pixels)}
{
    if (pixels_.size() != pixel_count(size)) {
        throw std::invalid_argument{"Pixel storage does not match dimensions"};
    }
}
PixelGrid::PixelGrid(PixelGrid&& other)
    : Widget{std::move(other)},
      odd_pixel{other.odd_pixel},
      size_{std::exchange(other.size_, Area{})},
      pixels_{std::move(other.pixels_)}
{ other.pixels_.clear(); }
auto PixelGrid::operator=(PixelGrid&& other) -> PixelGrid&
{
    if (this == &other) { return *this; }
    Widget::operator=(std::move(other));
    odd_pixel = other.odd_pixel;
    size_ = std::exchange(other.size_, Area{});
    pixels_ = std::move(other.pixels_);
    other.pixels_.clear();
    request_repaint();
    return *this;
}
auto PixelGrid::at(Point point) -> RGB& { return pixels_[pixel_index(size_, point)]; }
auto PixelGrid::at(Point point) const -> RGB const&
{ return pixels_[pixel_index(size_, point)]; }
auto PixelGrid::set_pixel(Point point, RGB color) -> bool
{
    auto& pixel = at(point);
    if (std::exchange(pixel, color) == color) { return false; }
    request_repaint();
    return true;
}
void PixelGrid::set_image(Area size, std::vector<RGB> pixels)
{
    if (pixels.size() != pixel_count(size)) {
        throw std::invalid_argument{"Pixel storage does not match dimensions"};
    }
    request_layout();
    size_ = size;
    pixels_.swap(pixels);
    request_repaint();
}
void PixelGrid::reset(Area size, RGB fill)
{
    auto pixels = std::vector<RGB>(pixel_count(size), fill);
    set_image(size, std::move(pixels));
}
auto PixelGrid::paint(Canvas canvas) -> Cursor
{
    paint_pixels(canvas, size_, pixels_, odd_pixel);
    return {};
}
}  // namespace ox
