#ifndef PAINTER_MODULE_DETAIL_PAINT_BUFFER_HPP
#define PAINTER_MODULE_DETAIL_PAINT_BUFFER_HPP

#include "painter_module/glyph_matrix.hpp"
#include <cstddef>

namespace twf {
namespace detail {

class Paint_buffer {
   public:
    explicit Paint_buffer(std::size_t x = 0, std::size_t y = 0)
        : backing_store_{x, y}, staging_area_{x, y} {}

    bool commit(std::size_t x, std::size_t y);
    void stage(std::size_t x, std::size_t y, const Glyph& glyph);

    void resize(std::size_t x, std::size_t y);
    const Glyph& at(std::size_t x, std::size_t y) const;
    std::size_t width() const { return staging_area_.width(); }
    std::size_t height() const { return staging_area_.height(); }

   private:
    Glyph_matrix backing_store_;
    Glyph_matrix staging_area_;
};

}  // namespace detail
}  // namespace twf
#endif  // PAINTER_MODULE_DETAIL_PAINT_BUFFER_HPP
