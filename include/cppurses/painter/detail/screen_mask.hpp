#ifndef CPPURSES_PAINTER_DETAIL_SCREEN_MASK_HPP
#define CPPURSES_PAINTER_DETAIL_SCREEN_MASK_HPP
#include <cstddef>
#include <vector>

#include <cppurses/widget/area.hpp>
#include <cppurses/widget/point.hpp>

namespace cppurses {
class Widget;
namespace detail {

/// A 2D bitmask to indicate a binary feature for each point on a Widget.
class Screen_mask {
   public:
    /// Create an empty Screen_mask with size (0,0).
    Screen_mask() = default;

    /// Create an empty Screen_mask with the dimensions and position of \p w.
    Screen_mask(const Widget& w);

    /// Return the offset of the Widget on the screen, top left point.
    Point offset() const;

    /// Return the area of the screen mask. Width and Height.
    Area area() const;

    /// Flip all bits in the mask.
    void flip();

    /// Retrieve the bit set at point \p p
    typename std::vector<bool>::reference at(std::size_t x, std::size_t y);

    /// Retrieve the bit set at point \p p
    typename std::vector<bool>::const_reference at(std::size_t x,
                                                   std::size_t y) const;

   private:
    Point offset_;  // subtract the offset from given point in at(Point)
    Area area_;     // for coordinate 2d to 1d and viceversa calculations
    std::vector<bool> bits_;
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_SCREEN_MASK_HPP
