#include <painter_module/detail/paint_buffer.hpp>
#include <painter_module/glyph.hpp>

namespace twf {
namespace detail {

bool Paint_buffer::commit(unsigned x, unsigned y) {
    if (staging_area_.at(x, y) == backing_store_.at(x, y)) {
        return false;
    }
    backing_store_.at(x, y) = staging_area_.at(x, y);
    return true;
}

void Paint_buffer::stage(unsigned x, unsigned y, const Glyph& glyph) {
    if (staging_area_.at(x, y) != glyph) {
        staging_area_.at(x, y) = glyph;
    }
}

// Glyph& Paint_buffer::at(unsigned x, unsigned y) { // not used.
//     return backing_store_.at(x, y);
// }

const Glyph& Paint_buffer::at(unsigned x, unsigned y) const {
    return backing_store_.at(x, y);
}

void Paint_buffer::resize(unsigned x, unsigned y) {
    backing_store_.resize(x, y);
    staging_area_.resize(x, y);
}

}  // namespace detail
}  // namespace twf
