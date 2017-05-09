#ifndef PAINT_BUFFER_HPP
#define PAINT_BUFFER_HPP

#include "../glyph_matrix.hpp"
#include <widget_module/coordinate.hpp>

namespace twf {
namespace detail {

class Paint_buffer {
   public:
    explicit Paint_buffer(unsigned x = 0, unsigned y = 0)
        : backing_store_{x, y}, staging_area_{x, y} {}

    void resize(unsigned x, unsigned y);

    unsigned width() const { return staging_area_.width(); }

    unsigned height() const { return staging_area_.height(); }

    bool commit(unsigned x, unsigned y);

    void stage(unsigned x, unsigned y, const Glyph& glyph);

    const Glyph& at(unsigned x, unsigned y) const;

    Coordinate cursor_position;

   private:
    // Glyph& at(unsigned x, unsigned y);

    Glyph_matrix backing_store_;
    Glyph_matrix staging_area_;
};

}  // namespace detail
}  // namespace twf
#endif  // PAINT_BUFFER_HPP
