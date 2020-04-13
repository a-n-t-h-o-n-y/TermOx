#ifndef CPPURSES_WIDGET_PIPE_HPP
#define CPPURSES_WIDGET_PIPE_HPP
#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

#include <cppurses/common/filter_iterator.hpp>
#include <cppurses/common/map_iterator.hpp>
#include <cppurses/common/range.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/animation_engine.hpp>
#include <cppurses/widget/align.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses::pipe::detail {

/// Used to call operator| overload to create a new Range from filter predicate
template <typename Predicate>
class Filter_predicate {
   public:
    Filter_predicate(Predicate p) : predicate{p} {}

   public:
    Predicate predicate;
};

/// Used to call operator| overload to create a new Range from filter predicate
template <typename W>
class Dynamic_filter_predicate {
   public:
    using Widget_t = W;
};

}  // namespace cppurses::pipe::detail

namespace cppurses {

/// Pipe operator for use with Widget.
template <
    typename Widget_t,
    typename F,
    typename std::enable_if_t<std::is_base_of_v<Widget, Widget_t>, int> = 0>
auto operator|(Widget_t& w, F&& op) -> std::invoke_result_t<F, Widget_t&>
{
    return std::forward<F>(op)(w);
}

/// Pipe operator for use with std::unique_ptr<Widget_t>.
template <
    typename Widget_t,
    typename F,
    typename std::enable_if_t<std::is_base_of_v<Widget, Widget_t>, int> = 0>
auto operator|(std::unique_ptr<Widget_t> w_ptr, F&& op)
    -> std::unique_ptr<Widget_t>
{
    std::forward<F>(op)(*w_ptr);
    return std::move(w_ptr);
}

/// Pipe operator for use with Widget::get_children.
template <typename Iter_1, typename Iter_2, typename F>
auto operator|(Range<Iter_1, Iter_2> children, F&& op) -> Range<Iter_1, Iter_2>
{
    for (auto& child : children)
        std::forward<F>(op)(child);
    return children;
}

/// Overload to create a filtered range.
template <typename Iter_1, typename Iter_2, typename F>
auto operator|(Range<Iter_1, Iter_2> children,
               pipe::detail::Filter_predicate<F>&& p)
{
    return Range{Filter_iterator{children.begin(), children.end(),
                                 std::forward<F>(p.predicate)},
                 children.end()};
}

// clang-format off
// clang format can't handle this one at the moment.
/// Overload to create a filtered range with upcast.
template <typename Iter_1, typename Iter_2, typename Widget_t>
auto operator|(Range<Iter_1, Iter_2> children,
               pipe::detail::Dynamic_filter_predicate<Widget_t>)
{
    return Range{
            Map_iterator{
                Filter_iterator{ children.begin(), children.end(),
                [](auto& w) { return dynamic_cast<Widget_t*>(&w) != nullptr; }},
                [](auto& w) -> auto& {return static_cast<Widget_t&>(w); }
                }, children.end()};
}
// clang-format on

/// Pipe operator for use with Widget::get_descendants.
template <typename F>
auto operator|(std::vector<Widget*> const& descendants, F&& op)
    -> std::vector<Widget*> const&
{
    for (auto* d : descendants)
        std::forward<F>(op)(*d);
    return descendants;
}

}  // namespace cppurses

namespace cppurses::pipe {

// Widget Accessors ------------------------------------------------------------
/// Widget -> Range<Children>
inline auto children()
{
    return [](auto& w) {
        auto c = w.get_children();
        return Range{c.begin(), c.end()};
    };
}

/// Widget -> std::vector<Widget*>
inline auto descendants()
{
    return [](auto& w) { return w.get_descendants(); };
}

// Generic Tools ---------------------------------------------------------------
template <typename F>
auto for_each(F&& f)
{
    return [&](auto& w) -> auto&
    {
        std::forward<F>(f)(w);
        return w;
    };
}

/// Container -> Container
template <typename F>
auto filter(F&& predicate)
{
    return detail::Filter_predicate{predicate};
}

/// Dynamic cast, be aware.
template <typename Widget_t>
auto filter()
{
    return detail::Dynamic_filter_predicate<Widget_t>{};
}

// Widget Modifiers ------------------------------------------------------------
inline auto name(std::string const& name)
{
    return [=](auto& w) -> auto&
    {
        w.set_name(std::move(name));
        return w;
    };
}

inline auto install_filter(Widget& filter)
{
    return [&](auto& w) -> auto&
    {
        w.install_event_filter(filter);
        return w;
    };
}

inline auto remove_filter(Widget& filter)
{
    return [&](auto& w) -> auto&
    {
        w.remove_event_filter(filter);
        return w;
    };
}

inline auto animate(Animation_engine::Period_t period)
{
    return [=](auto& w) -> auto&
    {
        w.enable_animation(period);
        return w;
    };
}

inline auto animate(
    std::function<Animation_engine::Period_t()> const& period_func)
{
    return [&](auto& w) -> auto&
    {
        w.enable_animation(period_func);
        return w;
    };
}

inline auto disanimate()
{
    return [](auto& w) -> auto&
    {
        w.disable_animation();
        return w;
    };
}

// Wallpaper Modifiers ---------------------------------------------------------
inline auto wallpaper(Glyph const& g)
{
    return [=](auto& w) -> auto&
    {
        w.set_wallpaper(g);
        return w;
    };
}

inline auto wallpaper(std::nullopt_t)
{
    return [](auto& w) -> auto&
    {
        w.set_wallpaper(std::nullopt);
        return w;
    };
}

inline auto wallpaper_with_brush()
{
    return [](auto& w) -> auto&
    {
        w.paint_wallpaper_with_brush(true);
        return w;
    };
}

inline auto wallpaper_without_brush()
{
    return [](auto& w) -> auto&
    {
        w.paint_wallpaper_with_brush(false);
        return w;
    };
}

// Brush Mofifiers -------------------------------------------------------------
inline auto bg(cppurses::Color c)
{
    return [=](auto& w) -> auto&
    {
        w.brush.set_background(c);
        w.update();
        return w;
    };
}

inline auto fg(cppurses::Color c)
{
    return [=](auto& w) -> auto&
    {
        w.brush.set_foreground(c);
        w.update();
        return w;
    };
}

inline auto remove_background()
{
    return [](auto& w) -> auto&
    {
        w.brush.remove_background();
        w.update();
        return w;
    };
}

inline auto remove_foreground()
{
    return [](auto& w) -> auto&
    {
        w.brush.remove_foreground();
        w.update();
        return w;
    };
}

inline auto add(Trait t)
{
    return [=](auto& w) -> auto&
    {
        w.brush.add_traits(t);
        w.update();
        return w;
    };
}

inline auto remove(Trait t)
{
    return [=](auto& w) -> auto&
    {
        w.brush.remove_traits(t);
        w.update();
        return w;
    };
}

inline auto clear_traits()
{
    return [](auto& w) -> auto&
    {
        w.brush.clear_traits();
        w.update();
        return w;
    };
}

// Cursor Modifiers ------------------------------------------------------------
inline auto show_cursor()
{
    return [](auto& w) -> auto&
    {
        w.cursor.enable();
        return w;
    };
}

inline auto hide_cursor()
{
    return [](auto& w) -> auto&
    {
        w.cursor.disable();
        return w;
    };
}

inline auto put_cursor(Point p)
{
    return [=](auto& w) -> auto&
    {
        w.cursor.set_position(p);
        return w;
    };
}

// Focus_policy ----------------------------------------------------------------
inline auto focus(Focus_policy p)
{
    return [=](auto& w) -> auto&
    {
        w.focus_policy = p;
        return w;
    };
}

inline auto no_focus() { return focus(Focus_policy::None); }

inline auto tab_focus() { return focus(Focus_policy::Tab); }

inline auto click_focus() { return focus(Focus_policy::Click); }

inline auto strong_focus() { return focus(Focus_policy::Strong); }

inline auto direct_focus() { return focus(Focus_policy::Direct); }

// Width Policy Modifiers -----------------------------------------------------

inline auto fixed_width(std::size_t hint)
{
    return [=](auto& w) -> auto&
    {
        w.width_policy.fixed(hint);
        return w;
    };
}

inline auto minimum_width(std::size_t hint)
{
    return [=](auto& w) -> auto&
    {
        w.width_policy.minimum(hint);
        return w;
    };
}

inline auto maximum_width(std::size_t hint)
{
    return [=](auto& w) -> auto&
    {
        w.width_policy.maximum(hint);
        return w;
    };
}

inline auto preferred_width(std::size_t hint)
{
    return [=](auto& w) -> auto&
    {
        w.width_policy.preferred(hint);
        return w;
    };
}

inline auto expanding_width(std::size_t hint)
{
    return [=](auto& w) -> auto&
    {
        w.width_policy.expanding(hint);
        return w;
    };
}

inline auto minimum_expanding_width(std::size_t hint)
{
    return [=](auto& w) -> auto&
    {
        w.width_policy.minimum_expanding(hint);
        return w;
    };
}

inline auto ignored_width()
{
    return [](auto& w) -> auto&
    {
        w.width_policy.ignored();
        return w;
    };
}

inline auto width_hint(std::size_t hint)
{
    return [=](auto& w) -> auto&
    {
        w.width_policy.hint(hint);
        return w;
    };
}

inline auto width_min(std::size_t min)
{
    return [=](auto& w) -> auto&
    {
        w.width_policy.min(min);
        return w;
    };
}

inline auto width_max(std::size_t max)
{
    return [=](auto& w) -> auto&
    {
        w.width_policy.max(max);
        return w;
    };
}

inline auto width_stretch(double stretch)
{
    return [=](auto& w) -> auto&
    {
        w.width_policy.max(stretch);
        return w;
    };
}

inline auto can_ignore_width_min()
{
    return [](auto& w) -> auto&
    {
        w.width_policy.can_ignore_min(true);
        return w;
    };
}

inline auto cannot_ignore_width_min()
{
    return [](auto& w) -> auto&
    {
        w.width_policy.can_ignore_min(false);
        return w;
    };
}

// Height Policy Modifiers -----------------------------------------------------

inline auto fixed_height(std::size_t hint)
{
    return [=](auto& w) -> auto&
    {
        w.height_policy.fixed(hint);
        return w;
    };
}

inline auto minimum_height(std::size_t hint)
{
    return [=](auto& w) -> auto&
    {
        w.height_policy.minimum(hint);
        return w;
    };
}

inline auto maximum_height(std::size_t hint)
{
    return [=](auto& w) -> auto&
    {
        w.height_policy.maximum(hint);
        return w;
    };
}

inline auto preferred_height(std::size_t hint)
{
    return [=](auto& w) -> auto&
    {
        w.height_policy.preferred(hint);
        return w;
    };
}

inline auto expanding_height(std::size_t hint)
{
    return [=](auto& w) -> auto&
    {
        w.height_policy.expanding(hint);
        return w;
    };
}

inline auto minimum_expanding_height(std::size_t hint)
{
    return [=](auto& w) -> auto&
    {
        w.height_policy.minimum_expanding(hint);
        return w;
    };
}

inline auto ignored_height()
{
    return [](auto& w) -> auto&
    {
        w.height_policy.ignored();
        return w;
    };
}

inline auto height_hint(std::size_t hint)
{
    return [=](auto& w) -> auto&
    {
        w.height_policy.hint(hint);
        return w;
    };
}

inline auto height_min(std::size_t min)
{
    return [=](auto& w) -> auto&
    {
        w.height_policy.min(min);
        return w;
    };
}

inline auto height_max(std::size_t max)
{
    return [=](auto& w) -> auto&
    {
        w.height_policy.max(max);
        return w;
    };
}

inline auto height_stretch(double stretch)
{
    return [=](auto& w) -> auto&
    {
        w.height_policy.max(stretch);
        return w;
    };
}

inline auto can_ignore_height_min()
{
    return [](auto& w) -> auto&
    {
        w.height_policy.can_ignore_min(true);
        return w;
    };
}

inline auto cannot_ignore_height_min()
{
    return [](auto& w) -> auto&
    {
        w.height_policy.can_ignore_min(false);
        return w;
    };
}

// Border Modifiers ------------------------------------------------------------
// Pre-Fab Border Shapes - most common of 256 total combinations
inline auto bordered()
{
    return [](auto& w) -> auto&
    {
        w.border.enable();
        auto& segments = w.border.segments;
        segments.north.enable();
        segments.south.enable();
        segments.east.enable();
        segments.west.enable();
        segments.north_east.enable();
        segments.north_west.enable();
        segments.south_east.enable();
        segments.south_west.enable();
        w.update();
        return w;
    };
}

inline auto not_bordered()
{
    return [](auto& w) -> auto&
    {
        w.border.disable();
        w.update();
        return w;
    };
}

inline auto north_border()
{
    return [](auto& w) -> auto&
    {
        w.border.enable();
        auto& segments = w.border.segments;
        segments.north.enable();
        segments.south.disable();
        segments.east.disable();
        segments.west.disable();
        segments.north_east.disable();
        segments.north_west.disable();
        segments.south_east.disable();
        segments.south_west.disable();
        w.update();
        return w;
    };
}

inline auto south_border()
{
    return [](auto& w) -> auto&
    {
        w.border.enable();
        auto& segments = w.border.segments;
        segments.north.disable();
        segments.south.enable();
        segments.east.disable();
        segments.west.disable();
        segments.north_east.disable();
        segments.north_west.disable();
        segments.south_east.disable();
        segments.south_west.disable();
        w.update();
        return w;
    };
}

inline auto east_border()
{
    return [](auto& w) -> auto&
    {
        w.border.enable();
        auto& segments = w.border.segments;
        segments.north.disable();
        segments.south.disable();
        segments.east.enable();
        segments.west.disable();
        segments.north_east.disable();
        segments.north_west.disable();
        segments.south_east.disable();
        segments.south_west.disable();
        w.update();
        return w;
    };
}

inline auto west_border()
{
    return [](auto& w) -> auto&
    {
        w.border.enable();
        auto& segments = w.border.segments;
        segments.north.disable();
        segments.south.disable();
        segments.east.disable();
        segments.west.enable();
        segments.north_east.disable();
        segments.north_west.disable();
        segments.south_east.disable();
        segments.south_west.disable();
        w.update();
        return w;
    };
}

inline auto north_east_border()
{
    return [](auto& w) -> auto&
    {
        w.border.enable();
        auto& segments = w.border.segments;
        segments.north.enable();
        segments.south.disable();
        segments.east.enable();
        segments.west.disable();
        segments.north_east.enable();
        segments.north_west.disable();
        segments.south_east.disable();
        segments.south_west.disable();
        w.update();
        return w;
    };
}

inline auto north_west_border()
{
    return [](auto& w) -> auto&
    {
        w.border.enable();
        auto& segments = w.border.segments;
        segments.north.enable();
        segments.south.disable();
        segments.east.disable();
        segments.west.enable();
        segments.north_east.disable();
        segments.north_west.enable();
        segments.south_east.disable();
        segments.south_west.disable();
        w.update();
        return w;
    };
}

inline auto south_east_border()
{
    return [](auto& w) -> auto&
    {
        w.border.enable();
        auto& segments = w.border.segments;
        segments.north.disable();
        segments.south.enable();
        segments.east.enable();
        segments.west.disable();
        segments.north_east.disable();
        segments.north_west.disable();
        segments.south_east.enable();
        segments.south_west.disable();
        w.update();
        return w;
    };
}

inline auto south_west_border()
{
    return [](auto& w) -> auto&
    {
        w.border.enable();
        auto& segments = w.border.segments;
        segments.north.disable();
        segments.south.enable();
        segments.east.disable();
        segments.west.enable();
        segments.north_east.disable();
        segments.north_west.disable();
        segments.south_east.disable();
        segments.south_west.enable();
        w.update();
        return w;
    };
}

inline auto north_south_border()
{
    return [](auto& w) -> auto&
    {
        w.border.enable();
        auto& segments = w.border.segments;
        segments.north.enable();
        segments.south.enable();
        segments.east.disable();
        segments.west.disable();
        segments.north_east.disable();
        segments.north_west.disable();
        segments.south_east.disable();
        segments.south_west.disable();
        w.update();
        return w;
    };
}

inline auto east_west_border()
{
    return [](auto& w) -> auto&
    {
        w.border.enable();
        auto& segments = w.border.segments;
        segments.north.disable();
        segments.south.disable();
        segments.east.enable();
        segments.west.enable();
        segments.north_east.disable();
        segments.north_west.disable();
        segments.south_east.disable();
        segments.south_west.disable();
        w.update();
        return w;
    };
}

inline auto corners_border()
{
    return [](auto& w) -> auto&
    {
        w.border.enable();
        auto& segments = w.border.segments;
        segments.north.disable();
        segments.south.disable();
        segments.east.disable();
        segments.west.disable();
        segments.north_east.enable();
        segments.north_west.enable();
        segments.south_east.enable();
        segments.south_west.enable();
        w.update();
        return w;
    };
}

inline auto no_corners_border()
{
    return [](auto& w) -> auto&
    {
        w.border.enable();
        auto& segments = w.border.segments;
        segments.north.enable();
        segments.south.enable();
        segments.east.enable();
        segments.west.enable();
        segments.north_east.disable();
        segments.north_west.disable();
        segments.south_east.disable();
        segments.south_west.disable();
        w.update();
        return w;
    };
}

inline auto no_walls_border()
{
    return [](auto& w) -> auto&
    {
        w.border.enable();
        auto& segments = w.border.segments;
        segments.north.disable();
        segments.south.disable();
        segments.east.disable();
        segments.west.disable();
        segments.north_east.enable();
        segments.north_west.enable();
        segments.south_east.enable();
        segments.south_west.enable();
        w.update();
        return w;
    };
}

namespace detail {
template <typename G>
using Can_make_glyph_from_t =
    std::enable_if_t<std::is_constructible_v<Glyph, G> ||
                         std::is_convertible_v<G, Glyph>,
                     int>;
}

// Wall/Corner Glyphs - Does not change border's enabled state.
template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto north_wall(G g)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north = g;
        w.update();
        return w;
    };
}

template <typename... Traits>
auto north_wall(Traits... a)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north.brush.add_traits(a...);
        w.update();
        return w;
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto south_wall(G g)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.south = g;
        w.update();
        return w;
    };
}

template <typename... Traits>
auto south_wall(Traits... a)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.south.brush.add_traits(a...);
        w.update();
        return w;
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto east_wall(G g)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.east = g;
        w.update();
        return w;
    };
}

template <typename... Traits>
auto east_wall(Traits... a)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.east.brush.add_traits(a...);
        w.update();
        return w;
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto west_wall(G g)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.west = g;
        w.update();
        return w;
    };
}

template <typename... Traits>
auto west_wall(Traits... a)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.west.brush.add_traits(a...);
        w.update();
        return w;
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto north_south_walls(G g)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north = g;
        w.border.segments.south = g;
        w.update();
        return w;
    };
}

template <typename... Traits>
auto north_south_walls(Traits... a)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north.brush.add_traits(a...);
        w.border.segments.south.brush.add_traits(a...);
        w.update();
        return w;
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto east_west_walls(G g)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.east = g;
        w.border.segments.west = g;
        w.update();
        return w;
    };
}

template <typename... Traits>
auto east_west_walls(Traits... a)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.east.brush.add_traits(a...);
        w.border.segments.west.brush.add_traits(a...);
        w.update();
        return w;
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto north_east_corner(G g)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north_east = g;
        w.update();
        return w;
    };
}

template <typename... Traits>
auto north_east_corner(Traits... a)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north_east.brush.add_traits(a...);
        w.update();
        return w;
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto north_east_walls(G g)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north      = g;
        w.border.segments.north_east = g;
        w.border.segments.east       = g;
        w.update();
        return w;
    };
}

template <typename... Traits>
auto north_east_walls(Traits... a)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north.brush.add_traits(a...);
        w.border.segments.north_east.brush.add_traits(a...);
        w.border.segments.east.brush.add_traits(a...);
        w.update();
        return w;
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto north_west_corner(G g)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north_west = g;
        w.update();
        return w;
    };
}

template <typename... Traits>
auto north_west_corner(Traits... a)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north_west.brush.add_traits(a...);
        w.update();
        return w;
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto north_west_walls(G g)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north      = g;
        w.border.segments.north_west = g;
        w.border.segments.west       = g;
        w.update();
        return w;
    };
}

template <typename... Traits>
auto north_west_walls(Traits... a)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north.brush.add_traits(a...);
        w.border.segments.north_west.brush.add_traits(a...);
        w.border.segments.west.brush.add_traits(a...);
        w.update();
        return w;
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto south_east_corner(G g)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.south_east = g;
        w.update();
        return w;
    };
}

template <typename... Traits>
auto south_east_corner(Traits... a)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.south_east.brush.add_traits(a...);
        w.update();
        return w;
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto south_east_walls(G g)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.south      = g;
        w.border.segments.south_east = g;
        w.border.segments.east       = g;
        w.update();
        return w;
    };
}

template <typename... Traits>
auto south_east_walls(Traits... a)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.south.brush.add_traits(a...);
        w.border.segments.south_east.brush.add_traits(a...);
        w.border.segments.east.brush.add_traits(a...);
        w.update();
        return w;
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto south_west_corner(G g)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.south_west = g;
        w.update();
        return w;
    };
}

template <typename... Traits>
auto south_west_corner(Traits... a)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.south_west.brush.add_traits(a...);
        w.update();
        return w;
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto south_west_walls(G g)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.south      = g;
        w.border.segments.south_west = g;
        w.border.segments.west       = g;
        w.update();
        return w;
    };
}

template <typename... Traits>
auto south_west_walls(Traits... a)
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.south.brush.add_traits(a...);
        w.border.segments.south_west.brush.add_traits(a...);
        w.border.segments.west.brush.add_traits(a...);
        w.update();
        return w;
    };
}

// Pre-Fab Border Glyphs - Does not change border's enabled state.
inline auto squared_corners()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north_east = L'┐';
        w.border.segments.north_west = L'┌';
        w.border.segments.south_east = L'┘';
        w.border.segments.south_west = L'└';
        w.update();
        return w;
    };
}

inline auto rounded_corners()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north_east = L'╮';
        w.border.segments.north_west = L'╭';
        w.border.segments.south_east = L'╯';
        w.border.segments.south_west = L'╰';
        w.update();
        return w;
    };
}

inline auto plus_corners()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north_east = L'+';
        w.border.segments.north_west = L'+';
        w.border.segments.south_east = L'+';
        w.border.segments.south_west = L'+';
        w.update();
        return w;
    };
}

inline auto asterisk_walls()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north      = L'*';
        w.border.segments.south      = L'*';
        w.border.segments.east       = L'*';
        w.border.segments.west       = L'*';
        w.border.segments.north_east = L'*';
        w.border.segments.north_west = L'*';
        w.border.segments.south_east = L'*';
        w.border.segments.south_west = L'*';
        w.update();
        return w;
    };
}

inline auto doubled_walls()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north      = L'═';
        w.border.segments.south      = L'═';
        w.border.segments.east       = L'║';
        w.border.segments.west       = L'║';
        w.border.segments.north_east = L'╗';
        w.border.segments.north_west = L'╔';
        w.border.segments.south_east = L'╝';
        w.border.segments.south_west = L'╚';
        w.update();
        return w;
    };
}

inline auto bold_walls()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north      = L'━';
        w.border.segments.south      = L'━';
        w.border.segments.east       = L'┃';
        w.border.segments.west       = L'┃';
        w.border.segments.north_east = L'┓';
        w.border.segments.north_west = L'┏';
        w.border.segments.south_east = L'┛';
        w.border.segments.south_west = L'┗';
        w.update();
        return w;
    };
}

inline auto dashed_walls_1()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north = L'╶';
        w.border.segments.south = L'╶';
        w.border.segments.east  = L'╷';
        w.border.segments.west  = L'╷';
        w.update();
        return w;
    };
}

inline auto bold_dashed_walls_1()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north = L'╺';
        w.border.segments.south = L'╺';
        w.border.segments.east  = L'╻';
        w.border.segments.west  = L'╻';
        w.update();
        return w;
    };
}

inline auto dashed_walls_2()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north = L'╌';
        w.border.segments.south = L'╌';
        w.border.segments.east  = L'╎';
        w.border.segments.west  = L'╎';
        w.update();
        return w;
    };
}

inline auto bold_dashed_walls_2()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north = L'╍';
        w.border.segments.south = L'╍';
        w.border.segments.east  = L'╏';
        w.border.segments.west  = L'╏';
        w.update();
        return w;
    };
}

inline auto dashed_walls_3()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north = L'┄';
        w.border.segments.south = L'┄';
        w.border.segments.east  = L'┆';
        w.border.segments.west  = L'┆';
        w.update();
        return w;
    };
}

inline auto bold_dashed_walls_3()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north = L'┅';
        w.border.segments.south = L'┅';
        w.border.segments.east  = L'┇';
        w.border.segments.west  = L'┇';
        w.update();
        return w;
    };
}

inline auto dashed_walls_4()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north = L'┈';
        w.border.segments.south = L'┈';
        w.border.segments.east  = L'┊';
        w.border.segments.west  = L'┊';
        w.update();
        return w;
    };
}

inline auto bold_dashed_walls_4()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north = L'┉';
        w.border.segments.south = L'┉';
        w.border.segments.east  = L'┋';
        w.border.segments.west  = L'┋';
        w.update();
        return w;
    };
}

inline auto block_walls_1()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north      = L'█';
        w.border.segments.south      = L'█';
        w.border.segments.east       = L'█';
        w.border.segments.west       = L'█';
        w.border.segments.north_east = L'█';
        w.border.segments.north_west = L'█';
        w.border.segments.south_east = L'█';
        w.border.segments.south_west = L'█';
        w.update();
        return w;
    };
}

inline auto block_walls_2()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north      = L'▓';
        w.border.segments.south      = L'▓';
        w.border.segments.east       = L'▓';
        w.border.segments.west       = L'▓';
        w.border.segments.north_east = L'▓';
        w.border.segments.north_west = L'▓';
        w.border.segments.south_east = L'▓';
        w.border.segments.south_west = L'▓';
        w.update();
        return w;
    };
}

inline auto block_walls_3()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north      = L'▒';
        w.border.segments.south      = L'▒';
        w.border.segments.east       = L'▒';
        w.border.segments.west       = L'▒';
        w.border.segments.north_east = L'▒';
        w.border.segments.north_west = L'▒';
        w.border.segments.south_east = L'▒';
        w.border.segments.south_west = L'▒';
        w.update();
        return w;
    };
}

inline auto block_walls_4()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north      = L'░';
        w.border.segments.south      = L'░';
        w.border.segments.east       = L'░';
        w.border.segments.west       = L'░';
        w.border.segments.north_east = L'░';
        w.border.segments.north_west = L'░';
        w.border.segments.south_east = L'░';
        w.border.segments.south_west = L'░';
        w.update();
        return w;
    };
}

inline auto half_block_walls()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north      = Glyph{L'▄', Trait::Inverse};
        w.border.segments.south      = L'▄';
        w.border.segments.east       = Glyph{L'▌', Trait::Inverse};
        w.border.segments.west       = L'▌';
        w.border.segments.north_east = L'▜';
        w.border.segments.north_west = L'▛';
        w.border.segments.south_east = L'▟';
        w.border.segments.south_west = L'▙';
        w.update();
        return w;
    };
}

inline auto half_block_inner_walls_1()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north      = L'▄';
        w.border.segments.south      = Glyph{L'▄', Trait::Inverse};
        w.border.segments.east       = L'▌';
        w.border.segments.west       = Glyph{L'▌', Trait::Inverse};
        w.border.segments.north_east = L'▖';
        w.border.segments.north_west = L'▗';
        w.border.segments.south_east = L'▘';
        w.border.segments.south_west = L'▝';
        w.update();
        return w;
    };
}

inline auto half_block_inner_walls_2()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north      = L'▄';
        w.border.segments.south      = Glyph{L'▄', Trait::Inverse};
        w.border.segments.east       = L'▌';
        w.border.segments.west       = Glyph{L'▌', Trait::Inverse};
        w.border.segments.north_east = L'▞';
        w.border.segments.north_west = L'▚';
        w.border.segments.south_east = L'▚';
        w.border.segments.south_west = L'▞';
        w.update();
        return w;
    };
}

inline auto block_corners()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north_east = L'▝';
        w.border.segments.north_west = L'▘';
        w.border.segments.south_east = L'▗';
        w.border.segments.south_west = L'▖';
        w.update();
        return w;
    };
}

inline auto floating_block_corners()
{
    return [=](auto& w) -> auto&
    {
        w.border.segments.north_east = L'▖';
        w.border.segments.north_west = L'▗';
        w.border.segments.south_east = L'▘';
        w.border.segments.south_west = L'▝';
        w.update();
        return w;
    };
}

// Widget::Signals -------------------------------------------------------------
template <typename Handler>
auto on_enable(Handler&& op)
{
    return [&](auto& w) -> auto&
    {
        w.enabled.connect(std::forward<Handler>(op));
        return w;
    };
}

template <typename Handler>
auto on_disable(Handler&& op)
{
    return [&](auto& w) -> auto&
    {
        w.disabled.connect(std::forward<Handler>(op));
        return w;
    };
}

template <typename Handler>
auto on_child_added(Handler&& op)
{
    return [&](auto& w) -> auto&
    {
        w.child_added.connect(std::forward<Handler>(op));
        return w;
    };
}

template <typename Handler>
auto on_child_removed(Handler&& op)
{
    return [&](auto& w) -> auto&
    {
        w.child_removed.connect(std::forward<Handler>(op));
        return w;
    };
}

template <typename Handler>
auto on_child_polished(Handler&& op)
{
    return [&](auto& w) -> auto&
    {
        w.child_polished.connect(std::forward<Handler>(op));
        return w;
    };
}

template <typename Handler>
auto on_move(Handler&& op)
{
    return [&](auto& w) -> auto&
    {
        w.moved.connect(std::forward<Handler>(op));
        return w;
    };
}

template <typename Handler>
auto on_resize(Handler&& op)
{
    return [&](auto& w) -> auto&
    {
        w.resized.connect(std::forward<Handler>(op));
        return w;
    };
}

template <typename Handler>
auto on_mouse_press(Handler&& op)
{
    return [&](auto& w) -> auto&
    {
        w.mouse_pressed.connect(std::forward<Handler>(op));
        return w;
    };
}

template <typename Handler>
auto on_left_click(Handler&& op)
{
    return [=](auto& w) -> auto&
    {
        w.mouse_pressed.connect([op](auto const& m) {
            if (m.button == Mouse::Button::Left)
                op();
        });
        return w;
    };
}

template <typename Handler>
auto on_middle_click(Handler&& op)
{
    return [=](auto& w) -> auto&
    {
        w.mouse_pressed.connect([op](auto const& m) {
            if (m.button == Mouse::Button::Middle)
                op();
        });
        return w;
    };
}

template <typename Handler>
auto on_right_click(Handler&& op)
{
    return [=](auto& w) -> auto&
    {
        w.mouse_pressed.connect([op](auto const& m) {
            if (m.button == Mouse::Button::Right)
                op();
        });
        return w;
    };
}

template <typename Handler>
auto on_mouse_release(Handler&& op)
{
    return [&](auto& w) -> auto&
    {
        w.mouse_released.connect(std::forward<Handler>(op));
        return w;
    };
}

template <typename Handler>
auto on_mouse_double_click(Handler&& op)
{
    return [&](auto& w) -> auto&
    {
        w.mouse_double_clicked.connect(std::forward<Handler>(op));
        return w;
    };
}

template <typename Handler>
auto on_mouse_move(Handler&& op)
{
    return [&](auto& w) -> auto&
    {
        w.mouse_moved.connect(std::forward<Handler>(op));
        return w;
    };
}

template <typename Handler>
auto on_key_press(Handler&& op)
{
    return [&](auto& w) -> auto&
    {
        w.key_pressed.connect(std::forward<Handler>(op));
        return w;
    };
}

template <typename Handler>
auto on_focus_in(Handler&& op)
{
    return [&](auto& w) -> auto&
    {
        w.focused_in.connect(std::forward<Handler>(op));
        return w;
    };
}

template <typename Handler>
auto on_focus_out(Handler&& op)
{
    return [&](auto& w) -> auto&
    {
        w.focused_out.connect(std::forward<Handler>(op));
        return w;
    };
}

template <typename Handler>
auto on_paint(Handler&& op)
{
    return [&](auto& w) -> auto&
    {
        w.painted.connect(std::forward<Handler>(op));
        return w;
    };
}

template <typename Handler>
auto on_timer(Handler&& op)
{
    return [&](auto& w) -> auto&
    {
        w.timer.connect(std::forward<Handler>(op));
        return w;
    };
}

template <typename Handler>
auto on_destroy(Handler&& op)
{
    return [&](auto& w) -> auto&
    {
        w.timer.connect(std::forward<Handler>(op));
        return w;
    };
}

// Derived Widget::Signals -----------------------------------------------------
template <typename Handler>
auto on_color_selected(Handler&& op)
{
    return [&](auto& w) -> auto&
    {
        w.color_selected.connect(std::forward<Handler>(op));
        return w;
    };
}

template <typename Handler>
auto on_press(Handler&& op)
{
    return [&](auto& w) -> auto&
    {
        w.pressed.connect(std::forward<Handler>(op));
        return w;
    };
}

// Derived Widget Modifiers ----------------------------------------------------
inline auto active_page(std::size_t p)
{
    return [=](auto& w) -> auto&
    {
        w.set_active_page(p);
        return w;
    };
}

inline auto label(Glyph_string const& x)
{
    return [=](auto& w) -> auto&
    {
        w.set_label(x);
        return w;
    };
}

// Labeled_cycle_box
inline auto divider(Glyph x)
{
    return [=](auto& w) -> auto&
    {
        w.set_divider(x);
        return w;
    };
}

inline auto word_wrap(bool enable)
{
    return [=](auto& w) -> auto&
    {
        w.enable_word_wrap(enable);
        return w;
    };
}

inline auto contents(Glyph_string x)
{
    return [=](auto& w) -> auto&
    {
        w.set_contents(x);
        return w;
    };
}

inline auto align_left()
{
    return [=](auto& w) -> auto&
    {
        w.set_alignment(Align::Left);
        return w;
    };
}

inline auto align_center()
{
    return [=](auto& w) -> auto&
    {
        w.set_alignment(Align::Center);
        return w;
    };
}

inline auto align_right()
{
    return [=](auto& w) -> auto&
    {
        w.set_alignment(Align::Right);
        return w;
    };
}

inline auto ghost(Color c)
{
    return [=](auto& w) -> auto&
    {
        w.set_ghost_color(c);
        return w;
    };
}

// Label
inline auto dynamic_width(bool enable)
{
    return [=](auto& w) -> auto&
    {
        w.set_dynamic_width(enable);
        return w;
    };
}

}  // namespace cppurses::pipe
#endif  // CPPURSES_WIDGET_PIPE_HPP
