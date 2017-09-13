#include "painter/detail/paint_buffer.hpp"
#include "painter/glyph.hpp"
#include <cstddef>

namespace cppurses {
namespace detail {

bool Paint_buffer::commit(std::size_t x, std::size_t y) {
    if (staging_area_.at(x, y) == backing_store_.at(x, y)) {
        return false;
    }
    backing_store_.at(x, y) = staging_area_.at(x, y);
    return true;
}

void Paint_buffer::stage(std::size_t x, std::size_t y, const Glyph& glyph) {
    if (y >= staging_area_.height() || x >= staging_area_.width()) {
        return;
    }
    if (staging_area_.at(x, y) != glyph) {
        staging_area_.at(x, y) = glyph;
    }
}

const Glyph& Paint_buffer::at(std::size_t x, std::size_t y) const {
    return backing_store_.at(x, y);
}

void Paint_buffer::resize(std::size_t x, std::size_t y) {
    backing_store_.resize(x, y);
    staging_area_.resize(x, y);
}

}  // namespace detail
}  // namespace cppurses
