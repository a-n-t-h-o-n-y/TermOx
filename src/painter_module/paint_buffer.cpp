#include <painter_module/detail/paint_buffer.hpp>
#include <painter_module/glyph.hpp>
#include <mutex>

namespace twf {
namespace detail {

bool Paint_buffer::commit(unsigned x, unsigned y) {
    std::lock_guard<std::mutex> bm(backing_mtx_);
    std::lock_guard<std::mutex> sm(staging_mtx_);
    if (staging_area_.at(x, y) == backing_store_.at(x, y)) {
        return false;
    }
    backing_store_.at(x, y) = staging_area_.at(x, y);
    return true;
}

void Paint_buffer::stage(unsigned x, unsigned y, const Glyph& glyph) {
    std::lock_guard<std::mutex> sm(staging_mtx_);
    if (staging_area_.at(x, y) != glyph) {
        staging_area_.at(x, y) = glyph;
    }
}

// Glyph& Paint_buffer::at(unsigned x, unsigned y) { // not used.
//     return backing_store_.at(x, y);
// }

const Glyph& Paint_buffer::at(unsigned x, unsigned y) const {
    std::lock_guard<std::mutex> bm(backing_mtx_);
    return backing_store_.at(x, y);
}

void Paint_buffer::resize(unsigned x, unsigned y) {
    std::lock_guard<std::mutex> bm(backing_mtx_);
    std::lock_guard<std::mutex> sm(staging_mtx_);
    backing_store_.resize(x, y);
    staging_area_.resize(x, y);
}

}  // namespace detail
}  // namespace twf
