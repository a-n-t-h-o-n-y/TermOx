#ifndef TERMOX_PAINTER_DETAIL_SCREEN_DESCRIPTOR_HPP
#define TERMOX_PAINTER_DETAIL_SCREEN_DESCRIPTOR_HPP
#include <unordered_map>

#include <termox/painter/glyph.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/point.hpp>

namespace ox::detail {

// using Screen_descriptor = std::unordered_map<Point, Glyph>;

/// Holds the screen state by Points on the screen and corresponding Glyphs.
/** Points are in global coordinates. */
class Screen_descriptor {
   private:
    using Map_t = std::unordered_map<Point, Glyph>;

   public:
    using key_type = Map_t::key_type;

   public:
    /// removes elements outside of \p area if resize is smaller than previous.
    void resize(Area a)
    {
        auto const old = area_;
        area_          = a;
        if (a.height < old.height || a.width < old.width) {
            auto iter           = std::begin(*this);
            auto const end      = std::end(*this);
            auto const new_edge = top_left_ + Point{area_.width, area_.height};
            while (iter != end) {
                auto const old_point = Point{iter->first};
                if (old_point.x < new_edge.x && old_point.y < new_edge.y)
                    ++iter;
                else
                    iter = this->erase(iter);
            }
        }
    }

    // This is only called after a clear() so don't bother resetting.
    void move(Point p) { top_left_ = p; }

    auto at(Point p) const -> Glyph { return map_.at(p); }

    auto operator[](Point&& p) -> Glyph& { return map_[std::move(p)]; }

    auto operator[](Point const& p) -> Glyph& { return map_[p]; }

    auto count(Point p) const -> std::size_t { return map_.count(p); }

    void clear() { map_.clear(); }

    auto begin() const -> Map_t::const_iterator { return std::begin(map_); }

    auto end() const -> Map_t::const_iterator { return std::end(map_); }

    auto erase(Map_t::const_iterator iter) -> Map_t::const_iterator
    {
        return map_.erase(iter);
    }

   private:
    Map_t map_;
    Area area_      = {0uL, 0uL};
    Point top_left_ = {0uL, 0uL};
};

}  // namespace ox::detail
#endif  // TERMOX_PAINTER_DETAIL_SCREEN_DESCRIPTOR_HPP
