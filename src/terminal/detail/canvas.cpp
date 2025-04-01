#include <termox/terminal/detail/canvas.hpp>

#include <algorithm>
#include <cassert>
#include <iterator>
#include <memory>
#include <ostream>
#include <vector>

#include <termox/common/u32_to_mb.hpp>
#include <termox/painter/brush.hpp>
#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/point.hpp>

namespace {

/// Return the next Point from \p p, within Area \p a.
/** p.x is incremented until a.width, where it is zeroed and p.y is inc. */
[[nodiscard]] auto next(ox::Point p, ox::Area a) -> ox::Point
{
    return (p.x + 1) < a.width ? ox::Point{p.x + 1, p.y}
                               : ox::Point{0, p.y + 1};
}

/// Return true if Point \p p is within the bounds of \p a.
[[nodiscard]] auto is_within(ox::Point p, ox::Area a) -> bool
{
    return (p.x < a.width) && (p.y < a.height);
}

}  // namespace

namespace ox::detail {

Canvas::Canvas(ox::Area a) : buffer_(a.width * a.height, ox::Glyph{}), area_{a}
{}

auto Canvas::area() const -> ox::Area { return area_; }

auto Canvas::at(ox::Point p) const -> ox::Glyph
{
    auto const index = p.x + (p.y * area_.width);
    assert(index < (int)buffer_.size());
    return buffer_[index];
}

auto Canvas::at(ox::Point p) -> ox::Glyph&
{
    auto const index = p.x + (p.y * area_.width);
    assert(index < (int)buffer_.size());
    return buffer_[index];
}

void Canvas::resize(ox::Area a)
{
    if (resize_buffer_ == nullptr)
        resize_buffer_ = std::make_unique<Canvas>(a);
    resize_buffer_->reset();
    resize_buffer_->area_ = a;
    resize_buffer_->buffer_.resize(a.width * a.height);
    auto current = ox::Point{0, 0};
    for (Glyph g : buffer_) {
        if (::is_within(current, a))
            resize_buffer_->at(current) = g;
        current = next(current, area_);
    }
    this->swap(*resize_buffer_);
}

auto Canvas::begin() -> Buffer_t::iterator { return std::begin(buffer_); }

auto Canvas::begin() const -> Buffer_t::const_iterator
{
    return std::cbegin(buffer_);
}

auto Canvas::end() -> Buffer_t::iterator { return std::end(buffer_); }

auto Canvas::end() const -> Buffer_t::const_iterator
{
    return std::cend(buffer_);
}

void Canvas::reset()
{
    std::fill(std::begin(buffer_), std::end(buffer_), Glyph{});
}

void Canvas::swap(Canvas& x)
{
    auto x_buf    = std::move(x.buffer_);
    auto x_area   = std::move(x.area_);
    x.buffer_     = std::move(this->buffer_);
    x.area_       = std::move(this->area_);
    this->buffer_ = std::move(x_buf);
    this->area_   = std::move(x_area);
}

void merge(Canvas const& next, Canvas& current)
{
    assert(next.area() == current.area());
    auto next_iter    = std::cbegin(next);
    auto current_iter = std::begin(current);
    for (; next_iter != std::cend(next); ++next_iter, ++current_iter) {
        if (next_iter->symbol != U'\0' && *next_iter != *current_iter)
            *current_iter = *next_iter;
    }
}

void merge_and_diff(Canvas const& next, Canvas& current, Canvas::Diff& diff_out)
{
    assert(next.area() == current.area());
    diff_out.clear();
    auto next_iter    = std::cbegin(next);
    auto current_iter = std::begin(current);
    auto point        = ox::Point{0, 0};
    for (; next_iter != std::cend(next); ++next_iter, ++current_iter) {
        if (next_iter->symbol != U'\0' && *next_iter != *current_iter) {
            diff_out.push_back({point, *next_iter});
            *current_iter = *next_iter;
        }
        point = ::next(point, next.area());
    }
}

void generate_color_diff(Color color,
                         Canvas const& canvas,
                         Canvas::Diff& diff_out)
{
    diff_out.clear();
    auto point = ox::Point{0, 0};
    for (Glyph g : canvas) {
        if (g.brush.foreground == color || g.brush.background == color)
            diff_out.push_back({point, g});
        point = next(point, canvas.area());
    }
}

void generate_full_diff(Canvas const& canvas, Canvas::Diff& diff_out)
{
    diff_out.clear();
    auto point = ox::Point{0, 0};
    for (Glyph g : canvas) {
        diff_out.push_back({point, g});
        point = next(point, canvas.area());
    }
}

auto print(Canvas::Diff const& diff, std::ostream& os) -> std::ostream&
{
    // using Diff = std::vector<std::pair<ox::Point, ox::Glyph>>;
    for (auto const& [point, glyph] : diff) {
        os << "Point: {" << point.x << ", " << point.y << "}\n";
        os << "Glyph: symbol: " << u32_to_mb(glyph.symbol) << '\n';
        os << "----------------------\n";
    }
    return os;
}

}  // namespace ox::detail
