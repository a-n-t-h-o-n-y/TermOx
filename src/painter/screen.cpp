#include <cppurses/painter/detail/screen.hpp>

#include <cstddef>
#include <iterator>
#include <mutex>

#include <optional/optional.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/detail/find_empty_space.hpp>
#include <cppurses/painter/detail/is_paintable.hpp>
#include <cppurses/painter/detail/ncurses_paint_engine.hpp>
#include <cppurses/painter/detail/screen_descriptor.hpp>
#include <cppurses/painter/detail/screen_mask.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace {
using namespace cppurses;

template <typename Map_t>
bool contains(const typename Map_t::key_type& value, const Map_t& map) {
    return map.count(value) > 0;
}

bool has_children(const Widget& widg) {
    return !(widg.children.get().empty());
}

bool is_whitespace_equal(const Brush& a, const Brush& b) {
    using Atr_t = Attribute;
    for (auto attr : {Atr_t::Underline, Atr_t::Standout, Atr_t::Inverse}) {
        if (a.has_attribute(attr) == b.has_attribute(attr)) {
            return false;
        }
    }
    return a.background_color() == b.background_color();
}

bool has_same_display(const Glyph& a, const Glyph& b) {
    return a == b || (a.symbol == L' ' && b.symbol == L' ' &&
                      is_whitespace_equal(a.brush, b.brush));
}

}  // namespace

namespace cppurses {
namespace detail {

void Screen::flush(const Staged_changes& changes) {
    bool refresh = false;
    for (const auto& widg_description : changes) {
        auto& widget = *widg_description.first;
        if (is_paintable(widget)) {
            delegate_paint(widget, widg_description.second);
            refresh = true;
        } else {
            widget.screen_state().tiles.clear();
        }
    }
    if (refresh) {
        engine_->refresh();
    }
}

void Screen::set_cursor_on_focus_widget() {
    auto* focus = Focus::focus_widget();
    if (focus != nullptr && focus->cursor.enabled() && is_paintable(*focus)) {
        engine_->show_cursor();
        const auto x_global = focus->inner_x() + focus->cursor.x();
        const auto y_global = focus->inner_y() + focus->cursor.y();
        engine_->move_cursor(x_global, y_global);
    } else {
        engine_->hide_cursor();
    }
}

// IMPLEMENTATION FUNCTIONS - - - - - - - - - - - - - - - - - - - - - - - - - -

void Screen::paint_empty_tiles(const Widget& widg) {
    if (!has_children(widg)) {
        return;
    }
    const auto wallpaper = widg.generate_wallpaper();
    const auto empty_space = find_empty_space(widg);
    const auto y_begin = empty_space.offset().y;
    const auto x_begin = empty_space.offset().x;
    const auto y_end = y_begin + empty_space.area().height;
    const auto x_end = x_begin + empty_space.area().width;
    for (auto y = y_begin; y < y_end; ++y) {
        for (auto x = x_begin; x < x_end; ++x) {
            if (empty_space.at(x, y)) {
                engine_->put(x, y, wallpaper);
            }
        }
    }
}

void Screen::cover_leftovers(Widget& widg,
                             const Screen_descriptor& staged_tiles) {
    const auto& wallpaper = widg.generate_wallpaper();
    auto& existing_tiles = widg.screen_state().tiles;
    for (auto iter = std::begin(existing_tiles);
         iter != std::end(existing_tiles);) {
        const auto& point = iter->first;
        if (!contains(point, staged_tiles)) {
            engine_->put(point.x, point.y, wallpaper);
            iter = existing_tiles.erase(iter);
        } else {
            ++iter;
        }
    }
}

void Screen::full_paint_single_point(Widget& widg,
                                     const Screen_descriptor& staged_tiles,
                                     const Point& point) {
    auto& existing_tiles = widg.screen_state().tiles;
    if (!contains(point, staged_tiles)) {
        if (!has_children(widg)) {
            engine_->put(point.x, point.y, widg.generate_wallpaper());
            existing_tiles.erase(point);
        }
        return;
    }
    auto tile = staged_tiles.at(point);
    imprint(widg.brush, tile.brush);
    if (!(contains(point, existing_tiles) && existing_tiles[point] == tile)) {
        engine_->put(point.x, point.y, tile);
        existing_tiles[point] = tile;
    }
}

void Screen::basic_paint_single_point(Widget& widg,
                                      const Point& point,
                                      Glyph tile) {
    imprint(widg.brush, tile.brush);
    auto& existing_tiles = widg.screen_state().tiles;
    if (!(contains(point, existing_tiles) && existing_tiles[point] == tile)) {
        engine_->put(point.x, point.y, tile);
        existing_tiles[point] = tile;
    }
}

void Screen::full_paint(Widget& widg, const Screen_descriptor& staged_tiles) {
    paint_empty_tiles(widg);
    const auto y_begin = widg.y();
    const auto x_begin = widg.x();
    const auto y_end = y_begin + widg.outer_height();
    const auto x_end = x_begin + widg.outer_width();
    for (auto y = y_begin; y < y_end; ++y) {
        for (auto x = x_begin; x < x_end; ++x) {
            full_paint_single_point(widg, staged_tiles, Point{x, y});
        }
    }
}

void Screen::basic_paint(Widget& widg, const Screen_descriptor& staged_tiles) {
    cover_leftovers(widg, staged_tiles);
    for (const auto& point_tile : staged_tiles) {
        basic_paint_single_point(widg, point_tile.first, point_tile.second);
    }
}

void Screen::paint_just_enabled(Widget& widg,
                                const Screen_descriptor& staged_tiles) {
    full_paint(widg, staged_tiles);
}

void Screen::paint_child_event(Widget& widg,
                               const Screen_descriptor& staged_tiles) {
    paint_empty_tiles(widg);
    basic_paint(widg, staged_tiles);
}

void Screen::paint_resize_event(Widget& widg,
                                const Screen_descriptor& staged_tiles) {
    paint_empty_tiles(widg);
    cover_leftovers(widg, staged_tiles);
    const auto& new_space = widg.screen_state().optimize.resize_mask;
    const auto y_begin = new_space.offset().y;
    const auto x_begin = new_space.offset().x;
    const auto y_end = y_begin + new_space.area().height;
    const auto x_end = x_begin + new_space.area().width;
    for (auto y = y_begin; y < y_end; ++y) {
        for (auto x = x_begin; x < x_end; ++x) {
            const Point point{x, y};
            if (new_space.at(x, y)) {
                full_paint_single_point(widg, staged_tiles, point);
            } else if (contains(point, staged_tiles)) {
                basic_paint_single_point(widg, point, staged_tiles.at(point));
            }
        }
    }
}

void Screen::paint_move_event(Widget& widg,
                              const Screen_descriptor& staged_tiles) {
    full_paint(widg, staged_tiles);
}

void Screen::delegate_paint(Widget& widg,
                            const Screen_descriptor& staged_tiles) {
    auto& optimization_info = widg.screen_state().optimize;
    auto& previous_wallpaper = optimization_info.wallpaper;
    const auto& current_wallpaper = widg.generate_wallpaper();
    if (optimization_info.just_enabled) {
        paint_just_enabled(widg, staged_tiles);
    } else if (!has_same_display(current_wallpaper, previous_wallpaper)) {
        full_paint(widg, staged_tiles);
    } else if (optimization_info.moved) {
        paint_move_event(widg, staged_tiles);
    } else if (optimization_info.resized) {
        paint_resize_event(widg, staged_tiles);
    } else if (optimization_info.child_event) {
        paint_child_event(widg, staged_tiles);
    } else {
        basic_paint(widg, staged_tiles);
    }
    optimization_info.reset();
    previous_wallpaper = current_wallpaper;
}

}  // namespace detail
}  // namespace cppurses
