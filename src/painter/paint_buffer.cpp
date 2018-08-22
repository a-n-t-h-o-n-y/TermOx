#include <cppurses/painter/paint_buffer.hpp>

#include <cstddef>
#include <mutex>

#include <cppurses/painter/detail/add_default_attributes.hpp>
#include <cppurses/painter/detail/find_empty_space.hpp>
#include <cppurses/painter/detail/is_not_paintable.hpp>
#include <cppurses/painter/detail/ncurses_data.hpp>
#include <cppurses/painter/detail/screen_mask.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_matrix.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/painter/palette.hpp>
#include <cppurses/painter/rgb.hpp>
#include <cppurses/system/detail/find_widget_at.hpp>
#include <cppurses/system/events/paint_event.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/widget.hpp>

// #define DEBUG_PAINT_BUFFER_CHILD_SIZES

#if defined(DEBUG_PAINT_BUFFER_CHILD_SIZES)
#include <fstream>
#endif

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

#if defined(DEBUG_PAINT_BUFFER_CHILD_SIZES)
void debug_child_size(const Widget* child) {
    std::ofstream l{"child_size_log.txt", std::ios::app};
    if (child == nullptr) {
        return;
    } else if (!child->enabled()) {
        return;
    }
    if (child->parent() == nullptr) {
        // head widget
        if (child->x() + child->outer_width() > System::max_width()) {
            l << "Head widget largest_x is too large" << std::endl;
        }
        if (child->y() + child->outer_height() > System::max_height()) {
            l << "Head widget largest_y is too large" << std::endl;
        }
    } else {
        const Widget* parent{child->parent()};
        // make sure smallest x in child is within parents x range.
        const std::size_t smallest_x{child->x()};
        bool out_of_bounds{false};
        if (smallest_x < parent->inner_x()) {
            out_of_bounds = true;
        }

        // make sure largest x in child is within parents x range.
        const std::size_t largest_x{child->x() + child->outer_width()};
        if (largest_x > parent->inner_x() + parent->width()) {
            out_of_bounds = true;
        }

        // make sure smallest y in child is within parents y range.
        const std::size_t smallest_y{child->y()};
        if (smallest_y < parent->inner_y()) {
            out_of_bounds = true;
        }

        // make sure largest y in child is within parents y range.
        const std::size_t largest_y{child->y() + child->outer_height()};
        if (largest_y > parent->inner_y() + parent->height()) {
            out_of_bounds = true;
        }
        if (out_of_bounds) {
            // child
            l << "child:\n";
            l << "(" << child->x() << ", " << child->y() << ")" << std::endl;
            l << "width: " << child->outer_width()
              << "\nheight: " << child->outer_height() << '\n';
            l << "---------\n";
            l << "parent:\n";
            l << "(" << parent->inner_x() << ", " << parent->inner_y() << ")"
              << std::endl;
            l << "width: " << parent->width()
              << "\nheight: " << parent->height() << '\n';
            l << "===========" << std::endl;

            // parent
        }
    }
    for (const std::unique_ptr<Widget>& c : child->children.get()) {
        debug_child_size(c.get());
    }
}
#endif
}  // namespace

namespace cppurses {

Paint_buffer::Paint_buffer() {
    this->update_width();
    this->update_height();
}

void Paint_buffer::paint_empty_tiles(Widget* w) {
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
                    engine_.put(x, y, wallpaper);
                }
            }
        }
    }
}

void Paint_buffer::full_paint_point(Widget* w,
                                    const detail::Screen_descriptor& changes,
                                    const Point& p) {
    Glyph wallpaper{w->generate_wallpaper()};
    if (changes.count(p) == 0 && !has_children(w)) {
        if (w->screen_state().tiles.count(p) == 1) {
            w->screen_state().tiles.erase(p);
        }
        engine_.put(p.x, p.y, wallpaper);
    }
    if (changes.count(p) == 1) {
        Glyph new_tile{changes.at(p)};
        detail::add_default_attributes(new_tile, w->brush);
        if (w->screen_state().tiles.count(p) == 1) {
            if (w->screen_state().tiles[p] == new_tile) {
                // no-op
            }
            if (w->screen_state().tiles[p] != new_tile) {
                engine_.put(p.x, p.y, new_tile);
                w->screen_state().tiles[p] = new_tile;
            }
        }
        if (w->screen_state().tiles.count(p) == 0) {
            engine_.put(p.x, p.y, new_tile);
            w->screen_state().tiles[p] = new_tile;
        }
    }
}

void Paint_buffer::full_paint(Widget* w,
                              const detail::Screen_descriptor& changes) {
    this->paint_empty_tiles(w);
    for (std::size_t y{w->y()}; y < (w->y() + w->outer_height()); ++y) {
        for (std::size_t x{w->x()}; x < (w->x() + w->outer_width()); ++x) {
            this->full_paint_point(w, changes, Point{x, y});
        }
    }
}

void Paint_buffer::paint_just_enabled(
    Widget* w,
    const detail::Screen_descriptor& changes) {
    this->full_paint(w, changes);
}

void Paint_buffer::paint_child_event(Widget* w,
                                     const detail::Screen_descriptor& changes) {
    this->paint_empty_tiles(w);
    this->basic_paint(w, changes);
}

void Paint_buffer::cover_leftovers(Widget* w,
                                   const detail::Screen_descriptor& changes) {
    const Glyph& wallpaper{w->generate_wallpaper()};
    detail::Screen_descriptor& screen_tiles{w->screen_state().tiles};
    for (auto it{std::begin(screen_tiles)}; it != std::end(screen_tiles);) {
        const Point& p{it->first};
        if (changes.count(p) == 0) {
            engine_.put(p.x, p.y, wallpaper);
            it = screen_tiles.erase(it);  // valid in C++14
        } else {
            ++it;
        }
    }
}

void Paint_buffer::basic_paint_point(Widget* w, const Point& p, Glyph tile) {
    detail::add_default_attributes(tile, w->brush);
    // Do not put a tile if screen state already has tile at p.
    auto screen_iter = w->screen_state().tiles.find(p);
    if (screen_iter != std::end(w->screen_state().tiles) &&
        screen_iter->second == tile) {
        return;
    } else {
        engine_.put(p.x, p.y, tile);
        w->screen_state().tiles[p] = tile;
    }
}

void Paint_buffer::basic_paint(Widget* w,
                               const detail::Screen_descriptor& changes) {
    this->cover_leftovers(w, changes);
    for (const auto& point_tile : changes) {
        basic_paint_point(w, point_tile.first, point_tile.second);
    }
}

// TODO Implement this.
void Paint_buffer::paint_move_event(Widget* w,
                                    const detail::Screen_descriptor& changes) {
    this->full_paint(w, changes);
}

void Paint_buffer::paint_resize_event(
    Widget* w,
    const detail::Screen_descriptor& changes) {
    this->paint_empty_tiles(w);

    this->cover_leftovers(w, changes);
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

void Paint_buffer::flush(const detail::Staged_changes& changes) {
    std::lock_guard<std::mutex> lock{detail::NCurses_data::ncurses_mtx};
    bool refresh{false};

#if defined(DEBUG_PAINT_BUFFER_CHILD_SIZES)
    debug_child_size(System::head());
#endif

    for (const auto& point_tile : changes) {
        Widget* w{point_tile.first};
        const detail::Screen_descriptor& changes{point_tile.second};

        if (detail::is_not_paintable(w)) {
            w->screen_state().tiles.clear();
            continue;
        }

        const Glyph& current_wallpaper{w->generate_wallpaper()};
        Glyph& last_wallpaper{w->screen_state().optimize.wallpaper};

        refresh = true;

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

    Widget* focus_w = Focus::focus_widget();
    if (focus_w == nullptr) {
        engine_.hide_cursor();
    } else if (!detail::is_not_paintable(focus_w)) {
        bool cursor_enabled{focus_w->cursor.enabled()};
        engine_.show_cursor(cursor_enabled);
        if (cursor_enabled) {
            std::size_t x = focus_w->inner_x() + focus_w->cursor.x();
            std::size_t y = focus_w->inner_y() + focus_w->cursor.y();
            engine_.move_cursor(x, y);
        }
    } else {
        engine_.hide_cursor();
    }
    if (refresh) {
        engine_.refresh();
    }
}

void Paint_buffer::move_cursor(std::size_t x, std::size_t y) {
    // std::lock_guard<Mutex_t> guard{mutex_};
    engine_.move_cursor(x, y);
}

void Paint_buffer::set_global_background_tile(const Glyph& tile) {
    // std::lock_guard<Mutex_t> guard{mutex_};
    global_background_tile_ = tile;
    // detail::repaint_all(); // TODO replace this with something.
}

Glyph Paint_buffer::get_global_background_tile() const {
    // std::lock_guard<Mutex_t> guard{mutex_};
    return global_background_tile_;
}

std::size_t Paint_buffer::update_width() {
    // std::lock_guard<std::mutex> guard{detail::NCurses_data::ncurses_mtx};
    width_ = engine_.screen_width();
    // std::size_t new_width{engine_.screen_width()};
    // this->resize_width(new_width);
    return width_;
}

std::size_t Paint_buffer::update_height() {
    // std::lock_guard<std::mutex> guard{detail::NCurses_data::ncurses_mtx};
    height_ = engine_.screen_height();
    // std::size_t new_height{engine_.screen_height()};
    // this->resize_height(new_height);
    return height_;
}

std::size_t Paint_buffer::screen_width() const {
    // std::lock_guard<Mutex_t> guard{mutex_};
    return width_;
}

std::size_t Paint_buffer::screen_height() const {
    // std::lock_guard<Mutex_t> guard{mutex_};
    return height_;
}

void Paint_buffer::set_color_definition(Color c, RGB values) {
    // std::lock_guard<Mutex_t> guard{mutex_};
    engine_.set_rgb(c, values.red, values.green, values.blue);
}

}  // namespace cppurses
