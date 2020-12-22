#ifndef TERMOX_PAINTER_DETAIL_SCREEN_MASK_HPP
#define TERMOX_PAINTER_DETAIL_SCREEN_MASK_HPP
#include <cstddef>
#include <vector>

#include <termox/widget/area.hpp>
#include <termox/widget/point.hpp>

namespace ox {
class Widget;
namespace detail {

/// A 2D bitmask to indicate a binary feature for each point on a Widget.
class Screen_mask {
   public:
    using Reference       = typename std::vector<bool>::reference;
    using Const_reference = typename std::vector<bool>::const_reference;

    enum Constructor_tag { Outer, Inner };

   public:
    /// Create an empty Screen_mask with size (0,0).
    Screen_mask() = default;

    /// Create an empty Screen_mask with the dimensions and position of \p w.
    Screen_mask(Widget const& w, Constructor_tag tag);

    /// Return the offset of the Widget on the screen, top left point.
    auto offset() const -> Point { return offset_; }

    /// Return the area of the screen mask. Width and Height.
    auto area() const -> Area { return area_; }

    /// Flip all bits in the mask.
    void flip() { bits_.flip(); }

    /// Return true if this->Area == {width: 0, height: 0}
    auto empty() const -> bool { return bits_.empty(); }

    /// Clear the contents to an empty state.
    void clear() { bits_.clear(); }

    /// Return the bit set at point \p p
    auto at(std::size_t x, std::size_t y) -> Reference
    {
        return bits_[index_at(x, y)];
    }

    /// Return the bit set at point \p p
    auto at(std::size_t x, std::size_t y) const -> Const_reference
    {
        return bits_[index_at(x, y)];
    }

   private:
    auto index_at(std::size_t x, std::size_t y) const -> std::size_t
    {
        x -= offset_.x;
        y -= offset_.y;
        return (y * area_.width) + x;
    }

   private:
    Point offset_;
    Area area_;
    std::vector<bool> bits_;
};

}  // namespace detail
}  // namespace ox
#endif  // TERMOX_PAINTER_DETAIL_SCREEN_MASK_HPP
