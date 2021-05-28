#ifndef TERMOX_WIDGET_PIPE_HPP
#define TERMOX_WIDGET_PIPE_HPP
#include <chrono>
#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <termox/common/filter_iterator.hpp>
#include <termox/common/overload.hpp>
#include <termox/common/range.hpp>
#include <termox/common/transform_iterator.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/align.hpp>
#include <termox/widget/bordered.hpp>
#include <termox/widget/detail/pipe_utility.hpp>
#include <termox/widget/focus_policy.hpp>
#include <termox/widget/growth.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/wrap.hpp>

namespace ox::pipe {

// Widget Accessors ------------------------------------------------------------
/// Widget -> Range<Children>
[[nodiscard]] inline auto children()
{
    return [](auto&& w) {
        auto c = get(w).get_children();
        return Range{c.begin(), c.end()};
    };
}

/// Widget -> std::vector<Widget*>
[[nodiscard]] inline auto descendants()
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
[[nodiscard]] auto filter(F&& predicate)
{
    return pipe::detail::Filter_predicate{predicate};
}

/// Filter by name of Widget
[[nodiscard]] inline auto find(std::string const& name)
{
    return pipe::detail::Filter_predicate{
        [=](auto const& w) { return w.name() == name; }};
}

/// Dynamic cast, be aware.
template <typename Widget_t>
[[nodiscard]] auto filter()
{
    return pipe::detail::Dynamic_filter_predicate<Widget_t>{};
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

inline auto animate(std::chrono::milliseconds interval)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).enable_animation(interval);
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

namespace ox {

// Brush Mofifiers -------------------------------------------------------------

/// Change the Background color of the given Widget.
template <typename Widget_t,
          typename std::enable_if_t<pipe::detail::is_widget_or_wptr<Widget_t>,
                                    int> = 0>
auto operator|(Widget_t&& w, Background_color bg) -> decltype(auto)
{
    get(w).brush.background = Color{bg.value};
    get(w).update();
    return std::forward<Widget_t>(w);
}

/// Change the Foreground color of the given Widget.
template <typename Widget_t,
          typename std::enable_if_t<pipe::detail::is_widget_or_wptr<Widget_t>,
                                    int> = 0>
auto operator|(Widget_t&& w, Foreground_color fg) -> decltype(auto)
{
    get(w).brush.foreground = Color{fg.value};
    get(w).update();
    return std::forward<Widget_t>(w);
}

/// Add \p t to the Brush of \p w.
template <typename Widget_t,
          typename std::enable_if_t<pipe::detail::is_widget_or_wptr<Widget_t>,
                                    int> = 0>
auto operator|(Widget_t&& w, Trait t) -> decltype(auto)
{
    get(w).brush.traits.insert(t);
    get(w).update();
    return std::forward<Widget_t>(w);
}

// Border Mofifiers ------------------------------------------------------------

template <
    typename Widget_t,
    typename std::enable_if_t<pipe::detail::is_widget_v<Widget_t>, int> = 0>
auto operator|(Bordered<Widget_t>& b, ox::border::Segments const& s)
    -> Bordered<Widget_t>&
{
    b.set(s);
    return b;
}

template <
    typename Widget_t,
    typename std::enable_if_t<pipe::detail::is_widget_v<Widget_t>, int> = 0>
auto operator|(std::unique_ptr<Bordered<Widget_t>> b_ptr,
               ox::border::Segments const& s)
    -> std::unique_ptr<Bordered<Widget_t>>
{
    b_ptr->set(s);
    return std::move(b_ptr);
}

template <
    typename Widget_t,
    typename std::enable_if_t<pipe::detail::is_widget_v<Widget_t>, int> = 0>
auto operator|(Bordered<Widget_t>& b, Background_color bg)
    -> Bordered<Widget_t>&
{
    b.set(b.segments() | bg);
    return b;
}

template <
    typename Widget_t,
    typename std::enable_if_t<pipe::detail::is_widget_v<Widget_t>, int> = 0>
auto operator|(std::unique_ptr<Bordered<Widget_t>> b_ptr, Background_color bg)
    -> std::unique_ptr<Bordered<Widget_t>>
{
    b_ptr->set(b_ptr->segments() | bg);
    return std::move(b_ptr);
}

template <
    typename Widget_t,
    typename std::enable_if_t<pipe::detail::is_widget_v<Widget_t>, int> = 0>
auto operator|(Bordered<Widget_t>& b, Foreground_color fg)
    -> Bordered<Widget_t>&
{
    b.set(b.segments() | fg);
    return b;
}

template <
    typename Widget_t,
    typename std::enable_if_t<pipe::detail::is_widget_v<Widget_t>, int> = 0>
auto operator|(std::unique_ptr<Bordered<Widget_t>> b_ptr, Foreground_color fg)
    -> std::unique_ptr<Bordered<Widget_t>>
{
    b_ptr->set(b_ptr->segments() | fg);
    return std::move(b_ptr);
}

}  // namespace ox

namespace ox::pipe {

inline auto drop_north()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).set(drop_north(get(w).segments()));
        return std::forward<decltype(w)>(w);
    };
}

inline auto drop_south()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).set(drop_south(get(w).segments()));
        return std::forward<decltype(w)>(w);
    };
}

inline auto drop_east()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).set(drop_east(get(w).segments()));
        return std::forward<decltype(w)>(w);
    };
}

inline auto drop_west()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).set(drop_west(get(w).segments()));
        return std::forward<decltype(w)>(w);
    };
}

inline auto take_north()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).set(take_north(get(w).segments()));
        return std::forward<decltype(w)>(w);
    };
}

inline auto take_south()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).set(take_south(get(w).segments()));
        return std::forward<decltype(w)>(w);
    };
}

inline auto take_east()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).set(take_east(get(w).segments()));
        return std::forward<decltype(w)>(w);
    };
}

inline auto take_west()
{
    return [](auto&& w) -> decltype(auto) {
        get(w).set(take_west(get(w).segments()));
        return std::forward<decltype(w)>(w);
    };
}

/// Discards Traits from Widgets or Glyph_strings
[[nodiscard]] inline auto discard(Traits ts)
{
    return Overload{[ts](Brush& b) -> Brush& {
                        b.traits.remove(ts);
                        return b;
                    },
                    [ts](Brush const& b) -> Brush {
                        auto copy = b;
                        copy.traits.remove(ts);
                        return copy;
                    },
                    [ts](Brush&& b) -> Brush {
                        b.traits.remove(ts);
                        return std::move(b);
                    },
                    [ts](Glyph& g) -> Glyph& {
                        g.brush.traits.remove(ts);
                        return g;
                    },
                    [ts](Glyph const& g) -> Glyph {
                        auto copy = g;
                        copy.brush.traits.remove(ts);
                        return copy;
                    },
                    [ts](Glyph&& g) -> Glyph {
                        g.brush.traits.remove(ts);
                        return std::move(g);
                    },
                    [ts](Glyph_string& gs) -> Glyph_string& {
                        gs.remove_traits(ts);
                        return gs;
                    },
                    [ts](Glyph_string const& gs) -> Glyph_string {
                        auto copy = gs;
                        copy.remove_traits(ts);
                        return copy;
                    },
                    [ts](Glyph_string&& gs) -> Glyph_string {
                        gs.remove_traits(ts);
                        return std::move(gs);
                    },
                    [ts](auto&& w) -> decltype(auto) {
                        get(w).brush.traits.remove(ts);
                        get(w).update();
                        return std::forward<decltype(w)>(w);
                    }};
}

inline auto clear_traits()
{
    return Overload{[](Brush& b) -> Brush& {
                        b.traits = Trait::None;
                        return b;
                    },
                    [](Brush b) -> Brush {
                        b.traits = Trait::None;
                        return b;
                    },
                    [](Glyph& g) -> Glyph& {
                        g.brush.traits = Trait::None;
                        return g;
                    },
                    [](Glyph const& g) -> Glyph {
                        auto copy         = g;
                        copy.brush.traits = Trait::None;
                        return copy;
                    },
                    [](Glyph&& g) -> Glyph {
                        g.brush.traits = Trait::None;
                        return std::move(g);
                    },
                    [](Glyph_string& gs) -> Glyph_string& {
                        gs.clear_traits();
                        return gs;
                    },
                    [](Glyph_string const& gs) -> Glyph_string {
                        auto copy = gs;
                        copy.clear_traits();
                        return copy;
                    },
                    [](Glyph_string&& gs) -> Glyph_string {
                        gs.clear_traits();
                        return std::move(gs);
                    },
                    [](auto&& w) -> decltype(auto) {
                        get(w).brush.traits = Trait::None;
                        get(w).update();
                        return std::forward<decltype(w)>(w);
                    }};
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

/// Give focus to \p receiver when the piped Widget gets focus_in_event.
inline auto forward_focus(Widget& receiver)
{
    return [&](auto&& w) -> decltype(auto) {
        get(w).focused_in.connect([&] { ::ox::System::set_focus(receiver); });
        return std::forward<decltype(w)>(w);
    };
}

// Width Policy Modifiers -----------------------------------------------------

inline auto fixed_width(int hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).width_policy.fixed(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto minimum_width(int hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).width_policy.minimum(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto maximum_width(int hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).width_policy.maximum(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto preferred_width(int hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).width_policy.preferred(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto expanding_width(int hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).width_policy.expanding(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto minimum_expanding_width(int hint)
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

inline auto width_hint(int hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).width_policy.hint(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto width_min(int min)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).width_policy.min(min);
        return std::forward<decltype(w)>(w);
    };
}

inline auto width_max(int max)
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

// Height Policy Modifiers -----------------------------------------------------

inline auto fixed_height(int hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).height_policy.fixed(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto minimum_height(int hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).height_policy.minimum(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto maximum_height(int hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).height_policy.maximum(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto preferred_height(int hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).height_policy.preferred(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto expanding_height(int hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).height_policy.expanding(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto minimum_expanding_height(int hint)
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

inline auto height_hint(int hint)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).height_policy.hint(hint);
        return std::forward<decltype(w)>(w);
    };
}

inline auto height_min(int min)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).height_policy.min(min);
        return std::forward<decltype(w)>(w);
    };
}

inline auto height_max(int max)
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
inline auto bind_key(Key k, Handler&& op)
{
    return [op = std::forward<Handler>(op), k](auto&& w) -> decltype(auto) {
        get(w).key_pressed.connect([&w, &op, k](auto pressed) {
            if (pressed == k)
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

inline auto wrapped()
{
    return [](auto&& w) -> auto& { return get(w).wrapped; };
}

// Labeled_cycle_box
inline auto divider(Glyph x)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).set_divider(x);
        return std::forward<decltype(w)>(w);
    };
}

inline auto any_wrap()
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).set_wrap(Wrap::Any);
        return std::forward<decltype(w)>(w);
    };
}

inline auto word_wrap()
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).set_wrap(Wrap::Word);
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

template <typename Number_t>
inline auto value(Number_t x)
{
    return [=](auto&& w) -> decltype(auto) {
        get(w).set_value(x);
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

/// Pipe operator for use with Widget.
template <
    typename Widget_t,
    typename F,
    typename = std::enable_if_t<pipe::detail::is_widget_or_wptr<Widget_t>, int>>
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

/// Overload to create a filtered range with upcast.
template <typename Iter_1, typename Iter_2, typename Widget_t>
auto operator|(Range<Iter_1, Iter_2> children,
               pipe::detail::Dynamic_filter_predicate<Widget_t>)
{
    // clang format can't handle this one at the moment.
    // clang-format off
    return Range{
            Transform_iterator{
                Filter_iterator{ children.begin(), children.end(),
                [](auto& w) { return dynamic_cast<Widget_t*>(&w) != nullptr; }},
                [](auto& w) -> auto& {return static_cast<Widget_t&>(w); }
                }, children.end()};
// clang-format on
}

/// Pipe operator for use with Widget::get_descendants.
template <typename F>
auto operator|(std::vector<Widget*> const& descendants, F&& op)
    -> std::vector<Widget*> const&
{
    for (auto* d : descendants)
        *d | op;
    return descendants;
}

/// Glyph_string pipe operator, for any function object pipe argument.
template <typename F>
auto operator|(Glyph_string& gs, F&& op)
    -> std::invoke_result_t<F, Glyph_string&>
{
    return std::forward<F>(op)(gs);
}

/// Glyph_string pipe operator, for any function object pipe argument.
template <typename F>
auto operator|(Glyph_string const& gs, F&& op)
    -> std::invoke_result_t<F, Glyph_string const&>
{
    return std::forward<F>(op)(gs);
}

/// Glyph_string pipe operator, for any function object pipe argument.
template <typename F>
auto operator|(Glyph_string&& gs, F&& op)
    -> std::invoke_result_t<F, Glyph_string&&>
{
    return std::forward<F>(op)(std::move(gs));
}

/// Glyph pipe operator, for any function object pipe argument.
template <typename F>
auto operator|(Glyph& g, F&& op) -> std::invoke_result_t<F, Glyph&>
{
    return std::forward<F>(op)(g);
}

/// Glyph pipe operator, for any function object pipe argument.
template <typename F>
auto operator|(Glyph const& g, F&& op) -> std::invoke_result_t<F, Glyph const&>
{
    return std::forward<F>(op)(g);
}

/// Glyph pipe operator, for any function object pipe argument.
template <typename F>
auto operator|(Glyph&& g, F&& op) -> std::invoke_result_t<F, Glyph&&>
{
    return std::forward<F>(op)(std::move(g));
}

/// Brush pipe operator, for any function object pipe argument.
template <typename F>
auto operator|(Brush& b, F&& op) -> std::invoke_result_t<F, Brush&>
{
    return std::forward<F>(op)(b);
}

/// Brush pipe operator, for any function object pipe argument.
template <typename F>
auto operator|(Brush const& b, F&& op) -> std::invoke_result_t<F, Brush const&>
{
    return std::forward<F>(op)(b);
}

/// Brush pipe operator, for any function object pipe argument.
template <typename F>
auto operator|(Brush&& b, F&& op) -> std::invoke_result_t<F, Brush&&>
{
    return std::forward<F>(op)(std::move(b));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_PIPE_HPP
