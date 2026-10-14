#pragma once

#include <cstddef>
#include <span>
#include <vector>

#include <ox/widget.hpp>

namespace ox {

/** Nonnegative dimensions and checked row-major arithmetic. */
[[nodiscard]] auto pixel_count(Area size) -> std::size_t;
[[nodiscard]] auto pixel_index(Area size, Point at) -> std::size_t;
/** Native terminal extent: one column per pixel, two pixel rows per cell. */
[[nodiscard]] auto pixel_cell_size(Area size) -> Area;
/** Validate shape, then draw only visible cells. The view can position/clip the image.
 * Odd final rows use odd_pixel for the missing lower half. Visible cells require a
 * one-column lower-half block in the canvas width policy. Does not fill margins. */
void paint_pixels(Canvas canvas,
                  Area size,
                  std::span<RGB const> pixels,
                  Color odd_pixel = RGB{0x000000});

/** An owned image composed from ordinary RGB storage and paint_pixels. */
class PixelGrid : public Widget {
   public:
    using Widget::at;
    using Widget::handle;
    Color odd_pixel = RGB{0x000000};

    explicit PixelGrid(Area size = {}, RGB fill = RGB{0x000000});
    PixelGrid(Area size, std::vector<RGB> pixels);

    PixelGrid(PixelGrid&& other);
    auto operator=(PixelGrid&& other) -> PixelGrid&;

    [[nodiscard]] auto pixel_size() const -> Area { return size_; }
    [[nodiscard]] auto pixels() -> std::span<RGB> { return pixels_; }
    [[nodiscard]] auto pixels() const -> std::span<RGB const> { return pixels_; }
    [[nodiscard]] auto at(Point point) -> RGB&;
    [[nodiscard]] auto at(Point point) const -> RGB const&;
    /** Direct span/reference edits need an application repaint request. */
    auto set_pixel(Point point, RGB color) -> bool;
    /** Replace or reset the whole image. Validate/allocate before committing. */
    void set_image(Area size, std::vector<RGB> pixels);
    void reset(Area size, RGB fill = RGB{0x000000});

    auto measure(Area available, esc::WidthPolicy = {}) const -> Area override
    {
        validate(available);
        return pixel_cell_size(size_);
    }
    auto paint(Canvas canvas) -> Cursor override;

   private:
    Area size_;
    std::vector<RGB> pixels_;
};
}  // namespace ox
