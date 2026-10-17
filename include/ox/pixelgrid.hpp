/** @file
 * Two-vertical-pixels-per-cell drawing with Unicode half blocks.
 */
#pragma once

#include <cstddef>
#include <span>
#include <vector>

#include <ox/widget.hpp>

namespace ox {

/** Nonnegative dimensions and checked row-major arithmetic. */
[[nodiscard]] auto pixel_count(Area size) -> std::size_t;
/** Validate a source-pixel coordinate and return its checked row-major index;
 * out-of-range points throw.
 */
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
    /** Color used for the absent lower pixel when the source has an odd number of rows. */
    Color odd_pixel = RGB{0x000000};

    /** Allocate source pixels initialized to fill; negative dimensions or overflow throw. */
    explicit PixelGrid(Area size = {}, RGB fill = RGB{0x000000});
    /** Own row-major RGB pixels. @throws std::invalid_argument Storage does not match
     * pixel_count(size).
     */
    PixelGrid(Area size, std::vector<RGB> pixels);

    /** Move widget identity and image storage; tracked references follow the move. */
    PixelGrid(PixelGrid&& other);
    /** Replace this identity and image with the source image and identity. */
    auto operator=(PixelGrid&& other) -> PixelGrid&;

    /** Return source-pixel dimensions, before two-pixel-row terminal encoding. */
    [[nodiscard]] auto pixel_size() const -> Area { return size_; }
    /** Borrow mutable source pixels until image replacement; direct edits require an
     * application repaint request.
     */
    [[nodiscard]] auto pixels() -> std::span<RGB> { return pixels_; }
    /** Borrow read-only row-major source pixels until image replacement. */
    [[nodiscard]] auto pixels() const -> std::span<RGB const> { return pixels_; }
    /** Borrow one checked source pixel; direct assignment requires an application
     * repaint request.
     */
    [[nodiscard]] auto at(Point point) -> RGB&;
    /** Borrow one checked source pixel read-only; invalid coordinates throw. */
    [[nodiscard]] auto at(Point point) const -> RGB const&;
    /** Assign a checked source pixel and request repaint if changed; return whether it
     * changed.
     */
    auto set_pixel(Point point, RGB color) -> bool;
    /** Replace or reset the whole image. Validate/allocate before committing. */
    void set_image(Area size, std::vector<RGB> pixels);
    /** Allocate a uniformly filled source image and request layout/repaint;
     * validation/allocation precede replacement.
     */
    void reset(Area size, RGB fill = RGB{0x000000});

    /** Return native terminal extent: one column per pixel and one row per pair of pixel
     * rows.
     */
    auto measure(Area available, esc::WidthPolicy = {}) const -> Area override
    {
        validate(available);
        return pixel_cell_size(size_);
    }
    /** Draw visible source pixels as lower-half blocks within the inherited clip; return
     * no caret.
     */
    auto paint(Canvas canvas) -> Cursor override;

   private:
    Area size_;
    std::vector<RGB> pixels_;
};
}  // namespace ox
