#pragma once
#include <vector>

#include <ox/widget.hpp>

namespace ox {

/**
 * An image-like widget that can be used to display a grid of pixels.
 * @details This uses the `▄` character to display two pixels per terminal cell.
 */
class PixelGrid : public Widget {
   public:
    struct Matrix {
        Area area;
        std::vector<TrueColor> pixels;

        [[nodiscard]]
        auto operator[](Point p) -> TrueColor&;

        [[nodiscard]]
        auto operator[](Point p) const -> TrueColor;
    } buffer;

   public:
    /**
     * Create a PixelGrid with the given dimensions.
     * @details The buffer is initialized to the given dimensions and filled with
     * 0x000000 pixels.
     */
    PixelGrid(Area area);

    /**
     * Create a PixelGrid from an existing matrix.
     */
    PixelGrid(Matrix matrix);

   public:
    void paint(Canvas c) override;
};

}  // namespace ox