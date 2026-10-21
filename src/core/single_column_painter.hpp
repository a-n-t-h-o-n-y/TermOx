#pragma once

#include <ox/core/canvas.hpp>

namespace ox::detail {
// Internal grid-painting helper. The caller validates the glyph widths before
// painting and visits only visible cells. Cache geometry for the whole batch;
// Frame still handles replacements that overlap existing wide graphemes.
class SingleColumnPainter {
   public:
    explicit SingleColumnPainter(Canvas canvas)
        : frame_{canvas.frame_}, origin_{canvas.at_}, clip_{canvas.global_clip()}
    {}
    void write(Point at, char32_t glyph, Style style) const
    {
        validate(style);
        frame_.write_validated(translate(at, origin_), {&glyph, 1}, style, clip_, 1);
    }

   private:
    Frame& frame_;
    Point origin_;
    Rect clip_;
};
}  // namespace ox::detail
