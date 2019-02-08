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
    using Reference = typename std::vector<bool>::reference;
    using Const_reference = typename std::vector<bool>::const_reference;

    enum Constructor_tag { Outer, Inner };

    /// Create an empty Screen_mask with size (0,0).
    Screen_mask() = default;

    /// Create an empty Screen_mask with the dimensions and position of \p w.
    Screen_mask(const Widget& w, Constructor_tag tag);

    /// Return the offset of the Widget on the screen, top left point.
    Point offset() const { return offset_; }

    /// Return the area of the screen mask. Width and Height.
    Area area() const { return area_; }

    /// Flip all bits in the mask.
    void flip() { bits_.flip(); }

    /// Return true if this->Area == {width: 0, height: 0}
    bool empty() const { return bits_.empty(); }

    /// Clear the contents to an empty state.
    void clear() { bits_.clear(); }

    /// Return the bit set at point \p p
    Reference at(std::size_t x, std::size_t y) { return bits_[index_at(x, y)]; }

    /// Return the bit set at point \p p
    Const_reference at(std::size_t x, std::size_t y) const {
        return bits_[index_at(x, y)];
    }

   private:
    Point offset_;
    Area area_;
    std::vector<bool> bits_;

    std::size_t index_at(std::size_t x, std::size_t y) const {
        x -= offset_.x;
        y -= offset_.y;
        return (y * area_.width) + x;
    }
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_SCREEN_MASK_HPP
