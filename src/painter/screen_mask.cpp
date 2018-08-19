#include <cppurses/painter/detail/screen_mask.hpp>

#include <stdexcept>
#include <vector>

#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
namespace detail {

// Just for access to top_left for offset, and width and height to set
// dimensions
Screen_mask::Screen_mask(const Widget& w)
    : offset_{w.x(), w.y()},
      area_{w.outer_width(), w.outer_height()},
      bits_(area_.width * area_.height) {}

Point Screen_mask::offset() const {
    return offset_;
}

Area Screen_mask::area() const {
    return area_;
}

void Screen_mask::flip() {
    bits_.flip();
}

typename std::vector<bool>::reference Screen_mask::at(std::size_t x,
                                                      std::size_t y) {
    // Bounds check
    if (x < offset_.x || y < offset_.y) {
        throw std::out_of_range{"Screen_mask::at()"};
    }
    // Offset
    x -= offset_.x;
    y -= offset_.y;

    // 2d to 1d transform
    std::size_t index{(y * area_.width) + x};

    return bits_.at(index);
    // return bits_[index];
}

typename std::vector<bool>::const_reference Screen_mask::at(
    std::size_t x,
    std::size_t y) const {
    // Bounds check
    if (x < offset_.x || y < offset_.y) {
        throw std::out_of_range{"Screen_mask::at()"};
    }
    // Offset
    x -= offset_.x;
    y -= offset_.y;

    // 2d to 1d transform
    std::size_t index{(y * area_.width) + x};

    return bits_.at(index);
    // return bits_[index];
}

}  // namespace detail
}  // namespace cppurses
