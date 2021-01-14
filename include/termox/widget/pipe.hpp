#ifndef TERMOX_WIDGET_PIPE_HPP
#define TERMOX_WIDGET_PIPE_HPP
#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <termox/common/filter_iterator.hpp>
#include <termox/common/range.hpp>
#include <termox/common/transform_iterator.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/system/animation_engine.hpp>
#include <termox/widget/align.hpp>
#include <termox/widget/focus_policy.hpp>
#include <termox/widget/growth.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>

namespace ox::pipe::detail {

/// Used to call operator| overload to create a new Range from filter predicate
template <typename Predicate>
class Filter_predicate {
   public:
    explicit Filter_predicate(Predicate p) : predicate{p} {}

   public:
    Predicate predicate;
};

/// Used to call operator| overload to create a new Range from filter predicate
template <typename W>
class Dynamic_filter_predicate {
   public:
    using Widget_t = W;
};

template <typename T>
struct is_widget_ptr : std::false_type {};

template <typename X>
struct is_widget_ptr<std::unique_ptr<X>> : std::is_base_of<ox::Widget, X> {};

/// True if T is a std::unique_ptr<> to a ox::\idget type.
template <typename T>
constexpr bool is_widget_ptr_v = is_widget_ptr<T>::value;

/// True if T is a Widget type.
template <typename T>
constexpr bool is_widget_v = std::is_base_of_v<Widget, std::decay_t<T>>;

/// True if T is a Widget type or a Widget pointer.
template <typename T>
constexpr bool is_widget_or_wptr =
    is_widget_v<T> || is_widget_ptr_v<std::decay_t<T>>;

}  // namespace ox::pipe::detail

namespace ox::pipe {

// Widget Accessors ------------------------------------------------------------
/// Widget -> Range<Children>
inline auto children()
{
    return [](auto&& w) {
        auto c = get(w).get_children();
        return Range{c.begin(), c.end()};
    };
}

/// Widget -> std::vector<Widget*>
inline auto descendants()
{
    return [](auto&& w) { return get(w).get_descendants(); };
}

// Generic Tools ---------------------------------------------------------------
template <typename F>
auto for_each(F&& f)
{
    return [&](auto&& w) -> decltype(auto) {
        std::forward<F>(f)(get(w));
        return std::forward<decltype(w)>(w);
    };
}

/// Container -> Container
template <typename F>
auto filter(F&& predicate)
{
    return detail::Filter_predicate{predicate};
}

/// Filter by name of Widget
inline auto find(std::string const& name)
{
    return detail::Filter_predicate{
        [=](auto const& w) { return w.name() == name; }};
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
    return [=](auto&& w) -> decltype(auto) {
        get(w).set_name(std::move(name));
        return std::forward<decltype(w)>(w);
    };
}

inline auto install_filter(Widget& filter)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).install_event_filter(filter);
        return std::forward<decltype(w)>(w);
    };
}

inline auto remove_filter(Widget& filter)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).remove_event_filter(filter);
        return std::forward<decltype(w)>(w);
    };
}

inline auto animate(Animation_engine::Period_t period)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).enable_animation(period);
        return std::forward<decltype(w)>(w);
    };
}

inline auto animate(FPS fps)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).enable_animation(fps);
        return std::forward<decltype(w)>(w);
    };
}

inline auto disanimate()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).disable_animation();
        return std::forward<decltype(w)>(w);
    };
}

// Wallpaper Modifiers ---------------------------------------------------------
inline auto wallpaper(Glyph g)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).set_wallpaper(g);
        return std::forward<decltype(w)>(w);
    };
}

inline auto wallpaper(std::nullopt_t)
{
    return [](auto&& w) -> decltype(auto) {
        get(w).set_wallpaper(std::nullopt);
        return std::forward<decltype(w)>(w);
    };
}

inline auto wallpaper_with_brush()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).paint_wallpaper_with_brush(true);
        return std::forward<decltype(w)>(w);
    };
}

inline auto wallpaper_without_brush()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).paint_wallpaper_with_brush(false);
        return std::forward<decltype(w)>(w);
    };
}

}  // namespace ox::pipe

// Brush Mofifiers -------------------------------------------------------------
namespace ox {

/// Change the Background color of the given Widget.
template <typename Widget_t,
          typename std::enable_if_t<pipe::detail::is_widget_or_wptr<Widget_t>,
                                    int> = 0>
auto operator|(Widget_t&& w, Background_color bg) -> decltype(auto)
{
    get(w).brush.add_traits(bg);
    get(w).update();
    return std::forward<Widget_t>(w);
}

/// Change the Foreground color of the given Widget.
template <typename Widget_t,
          typename std::enable_if_t<pipe::detail::is_widget_or_wptr<Widget_t>,
                                    int> = 0>
auto operator|(Widget_t&& w, Foreground_color fg) -> decltype(auto)
{
    get(w).brush.add_traits(fg);
    get(w).update();
    return std::forward<Widget_t>(w);
}

/// Add \p t to the Brush of \p w.
template <typename Widget_t,
          typename std::enable_if_t<pipe::detail::is_widget_or_wptr<Widget_t>,
                                    int> = 0>
auto operator|(Widget_t&& w, Trait t) -> decltype(auto)
{
    get(w).brush.add_traits(t);
    get(w).update();
    return std::forward<Widget_t>(w);
}

}  // namespace ox
namespace ox::pipe {

inline auto discard(Trait t)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).brush.remove_traits(t);
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto clear_traits()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).brush.clear_traits();
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

// Cursor Modifiers ------------------------------------------------------------
inline auto show_cursor()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).cursor.enable();
        return std::forward<decltype(w)>(w);
    };
}

inline auto hide_cursor()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).cursor.disable();
        return std::forward<decltype(w)>(w);
    };
}

inline auto put_cursor(Point p)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).cursor.set_position(p);
        return std::forward<decltype(w)>(w);
    };
}

// Focus_policy ----------------------------------------------------------------
inline auto focus(Focus_policy p)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).focus_policy = p;
        return std::forward<decltype(w)>(w);
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
    return [=](auto&& w) -> decltype(auto) {
        get(w).width_policy.fixed(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto minimum_width(std::size_t hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).width_policy.minimum(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto maximum_width(std::size_t hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).width_policy.maximum(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto preferred_width(std::size_t hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).width_policy.preferred(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto expanding_width(std::size_t hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).width_policy.expanding(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto minimum_expanding_width(std::size_t hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).width_policy.minimum_expanding(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto ignored_width()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).width_policy.ignored();
        return std::forward<decltype(w)>(w);
    };
}

inline auto width_hint(std::size_t hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).width_policy.hint(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto width_min(std::size_t min)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).width_policy.min(min);
        return std::forward<decltype(w)>(w);
    };
}

inline auto width_max(std::size_t max)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).width_policy.max(max);
        return std::forward<decltype(w)>(w);
    };
}

inline auto width_stretch(double stretch)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).width_policy.stretch(stretch);
        return std::forward<decltype(w)>(w);
    };
}

inline auto can_ignore_width_min()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).width_policy.can_ignore_min(true);
        return std::forward<decltype(w)>(w);
    };
}

inline auto cannot_ignore_width_min()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).width_policy.can_ignore_min(false);
        return std::forward<decltype(w)>(w);
    };
}

inline auto passive_width(bool x = true)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).width_policy.passive(x);
        return std::forward<decltype(w)>(w);
    };
}

// Height Policy Modifiers -----------------------------------------------------

inline auto fixed_height(std::size_t hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).height_policy.fixed(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto minimum_height(std::size_t hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).height_policy.minimum(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto maximum_height(std::size_t hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).height_policy.maximum(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto preferred_height(std::size_t hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).height_policy.preferred(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto expanding_height(std::size_t hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).height_policy.expanding(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto minimum_expanding_height(std::size_t hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).height_policy.minimum_expanding(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto ignored_height()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).height_policy.ignored();
        return std::forward<decltype(w)>(w);
    };
}

inline auto height_hint(std::size_t hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).height_policy.hint(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto height_min(std::size_t min)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).height_policy.min(min);
        return std::forward<decltype(w)>(w);
    };
}

inline auto height_max(std::size_t max)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).height_policy.max(max);
        return std::forward<decltype(w)>(w);
    };
}

inline auto height_stretch(double stretch)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).height_policy.stretch(stretch);
        return std::forward<decltype(w)>(w);
    };
}

inline auto can_ignore_height_min()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).height_policy.can_ignore_min(true);
        return std::forward<decltype(w)>(w);
    };
}

inline auto cannot_ignore_height_min()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).height_policy.can_ignore_min(false);
        return std::forward<decltype(w)>(w);
    };
}

inline auto passive_height(bool x = true)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).height_policy.passive(x);
        return std::forward<decltype(w)>(w);
    };
}

// Border Modifiers ------------------------------------------------------------
// Pre-Fab Border Shapes - most common of 256 total combinations
inline auto bordered()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).border.enable();
        auto& segments = get(w).border.segments;
        segments.north.enable();
        segments.south.enable();
        segments.east.enable();
        segments.west.enable();
        segments.north_east.enable();
        segments.north_west.enable();
        segments.south_east.enable();
        segments.south_west.enable();
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto not_bordered()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).border.disable();
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto north_border()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).border.enable();
        auto& segments = get(w).border.segments;
        segments.north.enable();
        segments.south.disable();
        segments.east.disable();
        segments.west.disable();
        segments.north_east.disable();
        segments.north_west.disable();
        segments.south_east.disable();
        segments.south_west.disable();
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto south_border()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).border.enable();
        auto& segments = get(w).border.segments;
        segments.north.disable();
        segments.south.enable();
        segments.east.disable();
        segments.west.disable();
        segments.north_east.disable();
        segments.north_west.disable();
        segments.south_east.disable();
        segments.south_west.disable();
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto east_border()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).border.enable();
        auto& segments = get(w).border.segments;
        segments.north.disable();
        segments.south.disable();
        segments.east.enable();
        segments.west.disable();
        segments.north_east.disable();
        segments.north_west.disable();
        segments.south_east.disable();
        segments.south_west.disable();
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto west_border()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).border.enable();
        auto& segments = get(w).border.segments;
        segments.north.disable();
        segments.south.disable();
        segments.east.disable();
        segments.west.enable();
        segments.north_east.disable();
        segments.north_west.disable();
        segments.south_east.disable();
        segments.south_west.disable();
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto north_east_border()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).border.enable();
        auto& segments = get(w).border.segments;
        segments.north.enable();
        segments.south.disable();
        segments.east.enable();
        segments.west.disable();
        segments.north_east.enable();
        segments.north_west.disable();
        segments.south_east.disable();
        segments.south_west.disable();
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto north_west_border()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).border.enable();
        auto& segments = get(w).border.segments;
        segments.north.enable();
        segments.south.disable();
        segments.east.disable();
        segments.west.enable();
        segments.north_east.disable();
        segments.north_west.enable();
        segments.south_east.disable();
        segments.south_west.disable();
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto south_east_border()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).border.enable();
        auto& segments = get(w).border.segments;
        segments.north.disable();
        segments.south.enable();
        segments.east.enable();
        segments.west.disable();
        segments.north_east.disable();
        segments.north_west.disable();
        segments.south_east.enable();
        segments.south_west.disable();
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto south_west_border()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).border.enable();
        auto& segments = get(w).border.segments;
        segments.north.disable();
        segments.south.enable();
        segments.east.disable();
        segments.west.enable();
        segments.north_east.disable();
        segments.north_west.disable();
        segments.south_east.disable();
        segments.south_west.enable();
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto north_south_border()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).border.enable();
        auto& segments = get(w).border.segments;
        segments.north.enable();
        segments.south.enable();
        segments.east.disable();
        segments.west.disable();
        segments.north_east.disable();
        segments.north_west.disable();
        segments.south_east.disable();
        segments.south_west.disable();
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto east_west_border()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).border.enable();
        auto& segments = get(w).border.segments;
        segments.north.disable();
        segments.south.disable();
        segments.east.enable();
        segments.west.enable();
        segments.north_east.disable();
        segments.north_west.disable();
        segments.south_east.disable();
        segments.south_west.disable();
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto corners_border()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).border.enable();
        auto& segments = get(w).border.segments;
        segments.north.disable();
        segments.south.disable();
        segments.east.disable();
        segments.west.disable();
        segments.north_east.enable();
        segments.north_west.enable();
        segments.south_east.enable();
        segments.south_west.enable();
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto no_corners_border()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).border.enable();
        auto& segments = get(w).border.segments;
        segments.north.enable();
        segments.south.enable();
        segments.east.enable();
        segments.west.enable();
        segments.north_east.disable();
        segments.north_west.disable();
        segments.south_east.disable();
        segments.south_west.disable();
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto no_walls_border()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).border.enable();
        auto& segments = get(w).border.segments;
        segments.north.disable();
        segments.south.disable();
        segments.east.disable();
        segments.west.disable();
        segments.north_east.enable();
        segments.north_west.enable();
        segments.south_east.enable();
        segments.south_west.enable();
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

namespace detail {
template <typename G>
using Can_make_glyph_from_t =
    std::enable_if_t<std::is_constructible_v<Glyph, G> ||
                         std::is_convertible_v<G, Glyph>,
                     int>;
}

/// Sets traits given to each wall of the border.
template <typename... Traits>
auto walls(Traits... a)
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x = get(w);
        x.border.segments.north.brush.add_traits(a...);
        x.border.segments.south.brush.add_traits(a...);
        x.border.segments.east.brush.add_traits(a...);
        x.border.segments.west.brush.add_traits(a...);
        x.border.segments.north_east.brush.add_traits(a...);
        x.border.segments.north_west.brush.add_traits(a...);
        x.border.segments.south_east.brush.add_traits(a...);
        x.border.segments.south_west.brush.add_traits(a...);
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

// Wall/Corner Glyphs - Does not change border's enabled state.
template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto north_wall(G g)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).border.segments.north = g;
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename... Traits>
auto north_wall(Traits... a)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).border.segments.north.brush.add_traits(a...);
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto south_wall(G g)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).border.segments.south = g;
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename... Traits>
auto south_wall(Traits... a)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).border.segments.south.brush.add_traits(a...);
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto east_wall(G g)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).border.segments.east = g;
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename... Traits>
auto east_wall(Traits... a)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).border.segments.east.brush.add_traits(a...);
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto west_wall(G g)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).border.segments.west = g;
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename... Traits>
auto west_wall(Traits... a)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).border.segments.west.brush.add_traits(a...);
        get(w).update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto north_south_walls(G g)
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                 = get(w);
        x.border.segments.north = g;
        x.border.segments.south = g;
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename... Traits>
auto north_south_walls(Traits... a)
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x = get(w);
        x.border.segments.north.brush.add_traits(a...);
        x.border.segments.south.brush.add_traits(a...);
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto east_west_walls(G g)
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                = get(w);
        x.border.segments.east = g;
        x.border.segments.west = g;
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename... Traits>
auto east_west_walls(Traits... a)
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x = get(w);
        x.border.segments.east.brush.add_traits(a...);
        x.border.segments.west.brush.add_traits(a...);
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto north_east_corner(G g)
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.north_east = g;
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename... Traits>
auto north_east_corner(Traits... a)
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x = get(w);
        x.border.segments.north_east.brush.add_traits(a...);
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto north_east_walls(G g)
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.north      = g;
        x.border.segments.north_east = g;
        x.border.segments.east       = g;
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename... Traits>
auto north_east_walls(Traits... a)
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x = get(w);
        x.border.segments.north.brush.add_traits(a...);
        x.border.segments.north_east.brush.add_traits(a...);
        x.border.segments.east.brush.add_traits(a...);
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto north_west_corner(G g)
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.north_west = g;
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename... Traits>
auto north_west_corner(Traits... a)
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x = get(w);
        x.border.segments.north_west.brush.add_traits(a...);
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto north_west_walls(G g)
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.north      = g;
        x.border.segments.north_west = g;
        x.border.segments.west       = g;
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename... Traits>
auto north_west_walls(Traits... a)
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x = get(w);
        x.border.segments.north.brush.add_traits(a...);
        x.border.segments.north_west.brush.add_traits(a...);
        x.border.segments.west.brush.add_traits(a...);
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto south_east_corner(G g)
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.south_east = g;
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename... Traits>
auto south_east_corner(Traits... a)
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x = get(w);
        x.border.segments.south_east.brush.add_traits(a...);
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto south_east_walls(G g)
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.south      = g;
        x.border.segments.south_east = g;
        x.border.segments.east       = g;
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename... Traits>
auto south_east_walls(Traits... a)
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x = get(w);
        x.border.segments.south.brush.add_traits(a...);
        x.border.segments.south_east.brush.add_traits(a...);
        x.border.segments.east.brush.add_traits(a...);
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto south_west_corner(G g)
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.south_west = g;
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename... Traits>
auto south_west_corner(Traits... a)
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x = get(w);
        x.border.segments.south_west.brush.add_traits(a...);
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename G, typename detail::Can_make_glyph_from_t<G> = 0>
auto south_west_walls(G g)
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.south      = g;
        x.border.segments.south_west = g;
        x.border.segments.west       = g;
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

template <typename... Traits>
auto south_west_walls(Traits... a)
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x = get(w);
        x.border.segments.south.brush.add_traits(a...);
        x.border.segments.south_west.brush.add_traits(a...);
        x.border.segments.west.brush.add_traits(a...);
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

// Pre-Fab Border Glyphs - Does not change border's enabled state.
inline auto squared_corners()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.north_east = L'┐';
        x.border.segments.north_west = L'┌';
        x.border.segments.south_east = L'┘';
        x.border.segments.south_west = L'└';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto rounded_corners()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.north_east = L'╮';
        x.border.segments.north_west = L'╭';
        x.border.segments.south_east = L'╯';
        x.border.segments.south_west = L'╰';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto plus_corners()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.north_east = L'+';
        x.border.segments.north_west = L'+';
        x.border.segments.south_east = L'+';
        x.border.segments.south_west = L'+';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto asterisk_walls()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.north      = L'*';
        x.border.segments.south      = L'*';
        x.border.segments.east       = L'*';
        x.border.segments.west       = L'*';
        x.border.segments.north_east = L'*';
        x.border.segments.north_west = L'*';
        x.border.segments.south_east = L'*';
        x.border.segments.south_west = L'*';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto doubled_walls()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.north      = L'═';
        x.border.segments.south      = L'═';
        x.border.segments.east       = L'║';
        x.border.segments.west       = L'║';
        x.border.segments.north_east = L'╗';
        x.border.segments.north_west = L'╔';
        x.border.segments.south_east = L'╝';
        x.border.segments.south_west = L'╚';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto bold_walls()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.north      = L'━';
        x.border.segments.south      = L'━';
        x.border.segments.east       = L'┃';
        x.border.segments.west       = L'┃';
        x.border.segments.north_east = L'┓';
        x.border.segments.north_west = L'┏';
        x.border.segments.south_east = L'┛';
        x.border.segments.south_west = L'┗';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto dashed_walls_1()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                 = get(w);
        x.border.segments.north = L'╶';
        x.border.segments.south = L'╶';
        x.border.segments.east  = L'╷';
        x.border.segments.west  = L'╷';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto bold_dashed_walls_1()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                 = get(w);
        x.border.segments.north = L'╺';
        x.border.segments.south = L'╺';
        x.border.segments.east  = L'╻';
        x.border.segments.west  = L'╻';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto dashed_walls_2()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                 = get(w);
        x.border.segments.north = L'╌';
        x.border.segments.south = L'╌';
        x.border.segments.east  = L'╎';
        x.border.segments.west  = L'╎';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto bold_dashed_walls_2()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                 = get(w);
        x.border.segments.north = L'╍';
        x.border.segments.south = L'╍';
        x.border.segments.east  = L'╏';
        x.border.segments.west  = L'╏';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto dashed_walls_3()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                 = get(w);
        x.border.segments.north = L'┄';
        x.border.segments.south = L'┄';
        x.border.segments.east  = L'┆';
        x.border.segments.west  = L'┆';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto bold_dashed_walls_3()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                 = get(w);
        x.border.segments.north = L'┅';
        x.border.segments.south = L'┅';
        x.border.segments.east  = L'┇';
        x.border.segments.west  = L'┇';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto dashed_walls_4()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                 = get(w);
        x.border.segments.north = L'┈';
        x.border.segments.south = L'┈';
        x.border.segments.east  = L'┊';
        x.border.segments.west  = L'┊';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto bold_dashed_walls_4()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                 = get(w);
        x.border.segments.north = L'┉';
        x.border.segments.south = L'┉';
        x.border.segments.east  = L'┋';
        x.border.segments.west  = L'┋';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto block_walls_1()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.north      = L'█';
        x.border.segments.south      = L'█';
        x.border.segments.east       = L'█';
        x.border.segments.west       = L'█';
        x.border.segments.north_east = L'█';
        x.border.segments.north_west = L'█';
        x.border.segments.south_east = L'█';
        x.border.segments.south_west = L'█';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto block_walls_2()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.north      = L'▓';
        x.border.segments.south      = L'▓';
        x.border.segments.east       = L'▓';
        x.border.segments.west       = L'▓';
        x.border.segments.north_east = L'▓';
        x.border.segments.north_west = L'▓';
        x.border.segments.south_east = L'▓';
        x.border.segments.south_west = L'▓';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto block_walls_3()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.north      = L'▒';
        x.border.segments.south      = L'▒';
        x.border.segments.east       = L'▒';
        x.border.segments.west       = L'▒';
        x.border.segments.north_east = L'▒';
        x.border.segments.north_west = L'▒';
        x.border.segments.south_east = L'▒';
        x.border.segments.south_west = L'▒';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto block_walls_4()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.north      = L'░';
        x.border.segments.south      = L'░';
        x.border.segments.east       = L'░';
        x.border.segments.west       = L'░';
        x.border.segments.north_east = L'░';
        x.border.segments.north_west = L'░';
        x.border.segments.south_east = L'░';
        x.border.segments.south_west = L'░';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto half_block_walls()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.north      = Glyph{L'▄', Trait::Inverse};
        x.border.segments.south      = L'▄';
        x.border.segments.east       = Glyph{L'▌', Trait::Inverse};
        x.border.segments.west       = L'▌';
        x.border.segments.north_east = L'▜';
        x.border.segments.north_west = L'▛';
        x.border.segments.south_east = L'▟';
        x.border.segments.south_west = L'▙';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto half_block_inner_walls_1()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.north      = L'▄';
        x.border.segments.south      = Glyph{L'▄', Trait::Inverse};
        x.border.segments.east       = L'▌';
        x.border.segments.west       = Glyph{L'▌', Trait::Inverse};
        x.border.segments.north_east = L'▖';
        x.border.segments.north_west = L'▗';
        x.border.segments.south_east = L'▘';
        x.border.segments.south_west = L'▝';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto half_block_inner_walls_2()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.north      = L'▄';
        x.border.segments.south      = Glyph{L'▄', Trait::Inverse};
        x.border.segments.east       = L'▌';
        x.border.segments.west       = Glyph{L'▌', Trait::Inverse};
        x.border.segments.north_east = L'▞';
        x.border.segments.north_west = L'▚';
        x.border.segments.south_east = L'▚';
        x.border.segments.south_west = L'▞';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto block_corners()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.north_east = L'▝';
        x.border.segments.north_west = L'▘';
        x.border.segments.south_east = L'▗';
        x.border.segments.south_west = L'▖';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

inline auto floating_block_corners()
{
    return [=](auto&& w) -> decltype(auto) {
        auto& x                      = get(w);
        x.border.segments.north_east = L'▖';
        x.border.segments.north_west = L'▗';
        x.border.segments.south_east = L'▘';
        x.border.segments.south_west = L'▝';
        x.update();
        return std::forward<decltype(w)>(w);
    };
}

// Widget::Signals -------------------------------------------------------------
template <typename Handler>
inline auto on_enable(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).enabled.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_disable(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).disabled.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_child_added(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).child_added.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_child_removed(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).child_removed.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_child_polished(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).child_polished.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_move(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).moved.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_resize(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).resized.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_mouse_press(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).mouse_pressed.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_left_click(Handler&& op)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).mouse_pressed.connect([op](auto const& m) {
            if (m.button == Mouse::Button::Left)
                op();
        });
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_middle_click(Handler&& op)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).mouse_pressed.connect([op](auto const& m) {
            if (m.button == Mouse::Button::Middle)
                op();
        });
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_right_click(Handler&& op)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).mouse_pressed.connect([op](auto const& m) {
            if (m.button == Mouse::Button::Right)
                op();
        });
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_mouse_release(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).mouse_released.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_mouse_double_click(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).mouse_double_clicked.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_mouse_move(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).mouse_moved.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_key_press(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).key_pressed.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto bind_key(Key key, Handler&& op)
{
    return [op = std::forward<Handler>(op), key](auto&& w) -> decltype(auto) {
        get(w).key_pressed.connect([&w, &op, key](auto pressed) {
            if (pressed == key)
                op(w);
        });
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_focus_in(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).focused_in.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_focus_out(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).focused_out.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_paint(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).painted.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_timer(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).timer.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_destroyed(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).destroyed.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

// Derived Widget::Signals -----------------------------------------------------
template <typename Handler>
inline auto on_color_selected(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).color_selected.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_press(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).pressed.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_check(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).checked.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_uncheck(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).unchecked.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

template <typename Handler>
inline auto on_toggle(Handler&& op)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).toggled.connect(std::forward<Handler>(op));
        return std::forward<decltype(w)>(w);
    };
}

// Derived Widget Modifiers ----------------------------------------------------
inline auto active_page(std::size_t p)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).set_active_page(p);
        return std::forward<decltype(w)>(w);
    };
}

inline auto label(Glyph_string const& x)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).set_label(x);
        return std::forward<decltype(w)>(w);
    };
}

// Labeled_cycle_box
inline auto divider(Glyph x)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).set_divider(x);
        return std::forward<decltype(w)>(w);
    };
}

inline auto word_wrap(bool enable)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).enable_word_wrap(enable);
        return std::forward<decltype(w)>(w);
    };
}

// TODO remove and replace with text()
inline auto contents(Glyph_string x)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).set_contents(x);
        return std::forward<decltype(w)>(w);
    };
}

inline auto text(Glyph_string x)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).set_text(x);
        return std::forward<decltype(w)>(w);
    };
}

inline auto align_left()
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).set_alignment(Align::Left);
        return std::forward<decltype(w)>(w);
    };
}

inline auto align_center()
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).set_alignment(Align::Center);
        return std::forward<decltype(w)>(w);
    };
}

inline auto align_right()
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).set_alignment(Align::Right);
        return std::forward<decltype(w)>(w);
    };
}

inline auto ghost(Color c)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).set_ghost_color(c);
        return std::forward<decltype(w)>(w);
    };
}

// Label
inline auto dynamic_growth()
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).set_growth_strategy(Growth::Dynamic);
        return std::forward<decltype(w)>(w);
    };
}

inline auto no_growth()
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).set_growth_strategy(Growth::Static);
        return std::forward<decltype(w)>(w);
    };
}

}  // namespace ox::pipe

namespace ox {

/// Return *x if x is a unique_ptr to a Widget type, otherwise x.
template <typename T>
constexpr auto get(T& x) -> auto&
{
    if constexpr (pipe::detail::is_widget_ptr_v<T>)
        return *x;
    else
        return x;
}

/// Pipe operator for use with Widget.
template <typename Widget_t,
          typename F,
          typename std::enable_if_t<pipe::detail::is_widget_or_wptr<Widget_t>,
                                    int> = 0>
auto operator|(Widget_t&& w, F&& op) -> std::invoke_result_t<F, Widget_t&&>
{
    return std::forward<F>(op)(std::forward<Widget_t>(w));
}

/// Pipe operator for use with Widget::get_children.
template <typename Iter_1, typename Iter_2, typename F>
auto operator|(Range<Iter_1, Iter_2> children, F&& op) -> Range<Iter_1, Iter_2>
{
    for (auto& child : children)
        child | op;
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
            Transform_iterator{
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
        *d | op;
    return descendants;
}

}  // namespace ox
#endif  // TERMOX_WIDGET_PIPE_HPP
