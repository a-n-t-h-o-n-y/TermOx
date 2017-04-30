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

}  // namespace detail
}  // namespace twf
