#include <cppurses/painter/detail/paint_middleman.hpp>

#include <cstddef>
#include <iterator>
#include <mutex>

#include <optional/optional.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/detail/add_default_attributes.hpp>
#include <cppurses/painter/detail/find_empty_space.hpp>
#include <cppurses/painter/detail/is_not_paintable.hpp>
#include <cppurses/painter/detail/ncurses_data.hpp>
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

bool has_children(const Widget* w) {
    return !w->children.get().empty();
}

bool wallpaper_changed(const Glyph& first, const Glyph& second) {
    if (first == second) {
        return false;
    } else if (first.symbol == L' ' && second.symbol == L' ') {
        if ((first.brush.background_color() ==
             second.brush.background_color()) &&
            (first.brush.has_attribute(Attribute::Underline) ==
             second.brush.has_attribute(Attribute::Underline)) &&
            (first.brush.has_attribute(Attribute::Standout) ==
             second.brush.has_attribute(Attribute::Standout)) &&
            (first.brush.has_attribute(Attribute::Inverse) ==
             second.brush.has_attribute(Attribute::Inverse))) {
            return false;
        }
    }
    return true;
}

}  // namespace

namespace cppurses {
namespace detail {

void Paint_middleman::flush(const detail::Staged_changes& changes) {
    std::lock_guard<std::mutex> lock{detail::NCurses_data::ncurses_mtx};
    bool refresh{false};
    for (const auto& point_tile : changes) {
        Widget* w{point_tile.first};
        const detail::Screen_descriptor& changes{point_tile.second};
        if (detail::is_not_paintable(w)) {
            w->screen_state().tiles.clear();
            continue;
        }
        refresh = true;
        const Glyph& current_wallpaper{w->generate_wallpaper()};
        Glyph& last_wallpaper{w->screen_state().optimize.wallpaper};
        if (w->screen_state().optimize.just_enabled) {
            this->paint_just_enabled(w, changes);
        } else if (wallpaper_changed(current_wallpaper, last_wallpaper)) {
            this->full_paint(w, changes);
        } else if (w->screen_state().optimize.moved) {
            this->paint_move_event(w, changes);
        } else if (w->screen_state().optimize.resized) {
            this->paint_resize_event(w, changes);
        } else if (w->screen_state().optimize.child_event) {
            this->paint_child_event(w, changes);
        } else {
            this->basic_paint(w, changes);
        }
        w->screen_state().optimize.reset();
        last_wallpaper = current_wallpaper;
    }
    if (refresh) {
        engine_->refresh();
    }
}

void Paint_middleman::set_cursor_on_focus_widget() {
    Widget* focus_w = Focus::focus_widget();
    if (focus_w == nullptr) {
        engine_->hide_cursor();
    } else if (!detail::is_not_paintable(focus_w)) {
        bool cursor_enabled{focus_w->cursor.enabled()};
        engine_->show_cursor(cursor_enabled);
        if (cursor_enabled) {
            std::size_t x = focus_w->inner_x() + focus_w->cursor.x();
            std::size_t y = focus_w->inner_y() + focus_w->cursor.y();
            engine_->move_cursor(x, y);
        }
    } else {
        engine_->hide_cursor();
    }
}

// IMPLEMENTATION FUNCTIONS - - - - - - - - - - - - - - - - - - - - - - - - - -

void Paint_middleman::paint_empty_tiles(Widget* w) {
    if (has_children(w)) {
        Glyph wallpaper{w->generate_wallpaper()};
        // create bitmask
        detail::Screen_mask mask{detail::find_empty_space(*w)};
        // Iterate over bitmask and put wallpapers
        for (std::size_t y{mask.offset().y};
             y < mask.offset().y + mask.area().height; ++y) {
            for (std::size_t x{mask.offset().x};
                 x < mask.offset().x + mask.area().width; ++x) {
                if (mask.at(x, y)) {
                    engine_->put(x, y, wallpaper);
                }
            }
        }
    }
}

void Paint_middleman::cover_leftovers(
    Widget* w,
    const detail::Screen_descriptor& changes) {
    const Glyph& wallpaper{w->generate_wallpaper()};
    detail::Screen_descriptor& screen_tiles{w->screen_state().tiles};
    for (auto it{std::begin(screen_tiles)}; it != std::end(screen_tiles);) {
        const Point& p{it->first};
        if (changes.count(p) == 0) {
            engine_->put(p.x, p.y, wallpaper);
            it = screen_tiles.erase(it);  // valid in C++14 for unordered_map
        } else {
            ++it;
        }
    }
}

void Paint_middleman::full_paint_point(Widget* w,
                                       const detail::Screen_descriptor& changes,
                                       const Point& p) {
    Glyph wallpaper{w->generate_wallpaper()};
    if (changes.count(p) == 0 && !has_children(w)) {
        if (w->screen_state().tiles.count(p) == 1) {
            w->screen_state().tiles.erase(p);
        }
        engine_->put(p.x, p.y, wallpaper);
    }
    if (changes.count(p) == 1) {
        Glyph new_tile{changes.at(p)};
        detail::add_default_attributes(new_tile, w->brush);
        if (w->screen_state().tiles.count(p) == 1) {
            if (w->screen_state().tiles[p] == new_tile) {
                // no-op
            }
            if (w->screen_state().tiles[p] != new_tile) {
                engine_->put(p.x, p.y, new_tile);
                w->screen_state().tiles[p] = new_tile;
            }
        }
        if (w->screen_state().tiles.count(p) == 0) {
            engine_->put(p.x, p.y, new_tile);
            w->screen_state().tiles[p] = new_tile;
        }
    }
}

void Paint_middleman::basic_paint_point(Widget* w, const Point& p, Glyph tile) {
    detail::add_default_attributes(tile, w->brush);
    // Do not put a tile if screen state already has tile at p.
    auto screen_iter = w->screen_state().tiles.find(p);
    if (screen_iter != std::end(w->screen_state().tiles) &&
        screen_iter->second == tile) {
        return;
    } else {
        engine_->put(p.x, p.y, tile);
        w->screen_state().tiles[p] = tile;
    }
}

void Paint_middleman::full_paint(Widget* w,
                                 const detail::Screen_descriptor& changes) {
    this->paint_empty_tiles(w);
    for (std::size_t y{w->y()}; y < (w->y() + w->outer_height()); ++y) {
        for (std::size_t x{w->x()}; x < (w->x() + w->outer_width()); ++x) {
            this->full_paint_point(w, changes, Point{x, y});
        }
    }
}

void Paint_middleman::basic_paint(Widget* w,
                                  const detail::Screen_descriptor& changes) {
    this->cover_leftovers(w, changes);
    for (const auto& point_tile : changes) {
        basic_paint_point(w, point_tile.first, point_tile.second);
    }
}

void Paint_middleman::paint_just_enabled(
    Widget* w,
    const detail::Screen_descriptor& changes) {
    this->full_paint(w, changes);
}

void Paint_middleman::paint_child_event(
    Widget* w,
    const detail::Screen_descriptor& changes) {
    this->paint_empty_tiles(w);
    this->basic_paint(w, changes);
}

void Paint_middleman::paint_resize_event(
    Widget* w,
    const detail::Screen_descriptor& changes) {
    this->paint_empty_tiles(w);

    this->cover_leftovers(w, changes);
    // Full paint of new points.
    const detail::Screen_mask& mask{w->screen_state().optimize.resize_mask};
    for (std::size_t y{mask.offset().y};
         y < mask.offset().y + mask.area().height; ++y) {
        for (std::size_t x{mask.offset().x};
             x < mask.offset().x + mask.area().width; ++x) {
            Point p{x, y};
            if (mask.at(p.x, p.y)) {
                this->full_paint_point(w, changes, p);
            } else if (changes.count(p) == 1) {
                this->basic_paint_point(w, p, changes.at(p));
            }
        }
    }
}

void Paint_middleman::paint_move_event(
    Widget* w,
    const detail::Screen_descriptor& changes) {
    this->full_paint(w, changes);
}

}  // namespace detail
}  // namespace cppurses
