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
    using Widget::handle;

    struct Matrix {
        Area area;
        std::vector<RGB> pixels;

        [[nodiscard]]
        auto operator[](Point p) -> RGB&;

        [[nodiscard]]
        auto operator[](Point p) const -> RGB;
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
    auto paint(Canvas c) -> Cursor override;
};

}  // namespace ox