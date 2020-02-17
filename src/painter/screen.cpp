#include <cppurses/painter/detail/screen.hpp>

#include <iterator>
#include <mutex>

#include <optional/optional.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/detail/find_empty_space.hpp>
#include <cppurses/painter/detail/is_paintable.hpp>
#include <cppurses/painter/detail/screen_descriptor.hpp>
#include <cppurses/painter/detail/screen_mask.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/system/detail/focus.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/terminal/output.hpp>
#include <cppurses/terminal/terminal.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace {
using namespace cppurses;

template <typename Map_t>
auto contains(typename Map_t::key_type const& value, Map_t const& map) -> bool
{
    return map.count(value) > 0;
}

auto has_children(Widget const& widg) -> bool
{
    return !(widg.get_children().empty());
}

auto is_whitespace_equal(Brush const& a, Brush const& b) -> bool
{
    using Atr_t = Attribute;
    for (auto attr : {Atr_t::Underline, Atr_t::Standout, Atr_t::Inverse}) {
        if (a.has_attribute(attr) == b.has_attribute(attr))
            return false;
    }
    return a.background_color() == b.background_color();
}

auto has_same_display(const Glyph& a, const Glyph& b) -> bool
{
    return a == b or (a.symbol == L' ' and b.symbol == L' ' and
                      is_whitespace_equal(a.brush, b.brush));
}

}  // namespace

namespace cppurses {
namespace detail {

void Screen::flush(Staged_changes::Map_t const& changes)
{
    auto refresh = false;
    for (auto const& widg_description : changes) {
        auto& widget = *widg_description.first;
        if (is_paintable(widget)) {
            delegate_paint(widget, widg_description.second);
            refresh = true;
        }
        else
            widget.screen_state().tiles.clear();
    }
    if (refresh)
        output::refresh();
}

void Screen::set_cursor_on_focus_widget()
{
    auto const* focus = detail::Focus::focus_widget();
    if (focus != nullptr and focus->cursor.enabled() and is_paintable(*focus)) {
        System::terminal.show_cursor();
        auto const x_global = focus->inner_x() + focus->cursor.x();
        auto const y_global = focus->inner_y() + focus->cursor.y();
        output::move_cursor(x_global, y_global);
    }
    else
        System::terminal.show_cursor(false);
}

void Screen::paint_empty_tiles(Widget const& widg)
{
    if (!has_children(widg))
        return;
    auto const wallpaper   = widg.generate_wallpaper();
    auto const empty_space = find_empty_space(widg);
    auto const y_begin     = empty_space.offset().y;
    auto const x_begin     = empty_space.offset().x;
    auto const y_end       = y_begin + empty_space.area().height;
    auto const x_end       = x_begin + empty_space.area().width;
    for (auto y = y_begin; y < y_end; ++y) {
        for (auto x = x_begin; x < x_end; ++x) {
            if (empty_space.at(x, y))
                output::put(x, y, wallpaper);
        }
    }
}

void Screen::cover_leftovers(Widget& widg,
                             Screen_descriptor const& staged_tiles)
{
    auto const& wallpaper = widg.generate_wallpaper();
    auto& existing_tiles  = widg.screen_state().tiles;
    for (auto iter = std::begin(existing_tiles);
         iter != std::end(existing_tiles);) {
        auto const& point = iter->first;
        if (contains(point, staged_tiles))
            ++iter;
        else {
            output::put(point.x, point.y, wallpaper);
            iter = existing_tiles.erase(iter);
        }
    }
}

void Screen::full_paint_single_point(Widget& widg,
                                     Screen_descriptor const& staged_tiles,
                                     Point const& point)
{
    auto& existing_tiles = widg.screen_state().tiles;
    if (!contains(point, staged_tiles)) {
        if (!has_children(widg)) {
            output::put(point.x, point.y, widg.generate_wallpaper());
            existing_tiles.erase(point);
        }
        return;
    }
    auto tile = staged_tiles.at(point);
    imprint(widg.brush, tile.brush);
    // if (!(contains(point, existing_tiles) and existing_tiles[point] == tile))
    // {
    output::put(point.x, point.y, tile);
    existing_tiles[point] = tile;
    // }
}

void Screen::basic_paint_single_point(Widget& widg,
                                      Point const& point,
                                      Glyph tile)
{
    imprint(widg.brush, tile.brush);
    auto& existing_tiles = widg.screen_state().tiles;
    if (!(contains(point, existing_tiles) and existing_tiles[point] == tile)) {
        output::put(point.x, point.y, tile);
        existing_tiles[point] = tile;
    }
}

void Screen::full_paint(Widget& widg, Screen_descriptor const& staged_tiles)
{
    paint_empty_tiles(widg);
    auto const y_begin = widg.y();
    auto const x_begin = widg.x();
    auto const y_end   = y_begin + widg.outer_height();
    auto const x_end   = x_begin + widg.outer_width();
    for (auto y = y_begin; y < y_end; ++y) {
        for (auto x = x_begin; x < x_end; ++x) {
            full_paint_single_point(widg, staged_tiles, Point{x, y});
        }
    }
}

void Screen::basic_paint(Widget& widg, Screen_descriptor const& staged_tiles)
{
    cover_leftovers(widg, staged_tiles);
    for (auto const& point_tile : staged_tiles) {
        basic_paint_single_point(widg, point_tile.first, point_tile.second);
    }
}

void Screen::paint_just_enabled(Widget& widg,
                                Screen_descriptor const& staged_tiles)
{
    full_paint(widg, staged_tiles);
}

void Screen::paint_child_event(Widget& widg,
                               const Screen_descriptor& staged_tiles)
{
    paint_empty_tiles(widg);
    basic_paint(widg, staged_tiles);
}

void Screen::paint_resize_event(Widget& widg,
                                const Screen_descriptor& staged_tiles)
{
    paint_empty_tiles(widg);
    cover_leftovers(widg, staged_tiles);
    const auto& new_space = widg.screen_state().optimize.resize_mask;
    const auto y_begin    = new_space.offset().y;
    const auto x_begin    = new_space.offset().x;
    const auto y_end      = y_begin + new_space.area().height;
    const auto x_end      = x_begin + new_space.area().width;
    for (auto y = y_begin; y < y_end; ++y) {
        for (auto x = x_begin; x < x_end; ++x) {
            const Point point{x, y};
            if (new_space.at(x, y)) {
                full_paint_single_point(widg, staged_tiles, point);
            }
            else if (contains(point, staged_tiles)) {
                basic_paint_single_point(widg, point, staged_tiles.at(point));
            }
        }
    }
}

void Screen::delegate_paint(Widget& widg, Screen_descriptor const& staged_tiles)
{
    auto& optimization_info       = widg.screen_state().optimize;
    auto& previous_wallpaper      = optimization_info.wallpaper;
    auto const& current_wallpaper = widg.generate_wallpaper();
    if (optimization_info.just_enabled)
        paint_just_enabled(widg, staged_tiles);
    else if (!has_same_display(current_wallpaper, previous_wallpaper))
        full_paint(widg, staged_tiles);
    else if (optimization_info.moved)
        paint_move_event(widg, staged_tiles);
    else if (optimization_info.resized) {
        // paint_resize_event(widg, staged_tiles); // should be this
        full_paint(widg, staged_tiles);
    }
    else if (optimization_info.child_event)
        paint_child_event(widg, staged_tiles);
    else
        basic_paint(widg, staged_tiles);
    optimization_info.reset();
    previous_wallpaper = current_wallpaper;
}

}  // namespace detail
}  // namespace cppurses
