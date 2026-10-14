#include <ox/core/canvas.hpp>

#include <cstdint>
#include <stdexcept>

namespace ox {
Canvas::Canvas(Frame& frame) : Canvas{frame, {0, 0}, frame.size()} {}
Canvas::Canvas(Frame& frame, Point at, Area size)
    : Canvas{frame, at, size, Rect{at, size}}
{}
Canvas::Canvas(Frame& frame, Point at, Area size, Rect clip)
    : frame_{frame}, at_{at}, size_{size}, clip_{intersection(Rect{at, size}, clip)}
{ validate(size); }

auto Canvas::global_clip() const -> Rect
{ return intersection(clip_, Rect{{0, 0}, frame_.size()}); }

auto Canvas::visible() const -> Rect
{
    auto clip = this->global_clip();
    if (clip.size.width == 0 || clip.size.height == 0) { return {{0, 0}, {0, 0}}; }
    return {{(int)((std::int64_t)clip.at.x - at_.x),
             (int)((std::int64_t)clip.at.y - at_.y)},
            clip.size};
}

auto Canvas::contains(Point point) const -> bool
{ return ox::contains(this->visible(), point); }
auto Canvas::to_global(Point point) const -> Point { return translate(at_, point); }

auto Canvas::operator[](Point point) const -> Cell const&
{
    if (!this->contains(point)) {
        throw std::out_of_range{"Canvas coordinate outside clip"};
    }
    return frame_[this->to_global(point)];
}

auto Canvas::subcanvas(Point at, Area size) const -> Canvas
{ return Canvas{frame_, this->to_global(at), size, clip_}; }

auto Canvas::write(Point at, std::u32string_view grapheme, Style style) const -> bool
{
    validate(style);
    // Validate even clipped input, so visibility never changes text validity.
    auto width = esc::grapheme_width(grapheme, this->width_policy());
    if (!width || *width == 0) {
        throw std::invalid_argument{"Expected a printable grapheme"};
    }
    if (!this->contains(at)) { return false; }
    return frame_.write(this->to_global(at), grapheme, style, this->global_clip());
}

auto Canvas::restyle(Point at, Style style) const -> bool
{
    validate(style);
    if (!this->contains(at)) { return false; }
    return frame_.restyle(this->to_global(at), style, this->global_clip());
}
}  // namespace ox
