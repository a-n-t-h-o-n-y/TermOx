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
    for (const std::unique_ptr<Widget>& child : w->children.get()) {
        if (child->enabled()) {
            return true;
        }
    }
    return false;
}

bool child_has_point(const Widget& w, std::size_t x, std::size_t y) {
    const std::vector<std::unique_ptr<Widget>>& children{w.children.get()};
    for (const std::unique_ptr<Widget>& child : children) {
        if (x >= child->x() && x < (child->inner_x() + child->outer_width()) &&
            y >= child->y() && y < (child->inner_y() + child->outer_height())) {
            return true;
        }
    }
    return false;
}

bool wallpaper_changed(const Glyph& first, const Glyph& second) {
    if (first == second) {
        return false;
        // attrs are same except foreground color)
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
        // l << "child == nullptr" << std::endl;
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
            // l << child << " child left x is less than parent left x\n";
            // l << "child smallest_x: " << smallest_x << " parent smallest_x:
            // "; l << parent->x() << std::endl;
        }

        // make sure largest x in child is within parents x range.
        const std::size_t largest_x{child->x() + child->outer_width()};
        if (largest_x > parent->inner_x() + parent->width()) {
            out_of_bounds = true;
            // l << child << " child largest_x is greater than parent
            // largest_x\n"; l << "child largest_x: " << largest_x << " parent
            // largest_x: "; l << parent->x() + parent->outer_width() <<
            // std::endl;
        }

        // -------------------------------------------------------

        // make sure smallest y in child is within parents y range.
        const std::size_t smallest_y{child->y()};
        if (smallest_y < parent->inner_y()) {
            out_of_bounds = true;
            // l << child << " child left y is less than parent left y\n";
            // l << "child smallest_y: " << smallest_y << " parent smallest_y:
            // "; l << parent->y() << std::endl;
        }

        // make sure largest y in child is within parents y range.
        const std::size_t largest_y{child->y() + child->outer_height()};
        if (largest_y > parent->inner_y() + parent->height()) {
            out_of_bounds = true;
            // l << child << " child largest_y is greater than parent
            // largest_y\n"; l << "child largest_y: " << largest_y << " parent
            // largest_y: "; l << parent->y() + parent->outer_height() <<
            // std::endl;
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

// bool Paint_buffer::within_screen(const Point& p) {
//     if (p.x >= this->screen_width() || p.y >= this->screen_height()) {
//         return false;
//     }
//     return true;
// }

// void Paint_buffer::cover_all_with_background() {
//     for (std::size_t y{0}; y < (System::max_height()); ++y) {
//         for (std::size_t x{0}; x < (System::max_width()); ++x) {
//             Widget* w{detail::find_widget_at(x, y)};
//             // TODO throw here if == nullptr, for debugging.
//             if (w != nullptr) {
//                 Glyph background{w->find_background_tile()};
//                 engine_.put(x, y, background);
//             }
//         }
//     }
// }

// void Paint_buffer::cover_with_background(Widget& w) {
//     Glyph background{w.find_background_tile()};
//     for (std::size_t y{w.y()}; y < (w.y() + w.outer_height()); ++y) {
//         for (std::size_t x{w.x()}; x < (w.x() + w.outer_width()); ++x) {
//             if (!child_has_point(w, x, y)) {
//                 engine_.put(x, y, background);
//             }
//         }
//     }
// }

void Paint_buffer::full_paint(Widget* w,
                              const detail::Screen_descriptor& changes) {
    Glyph wallpaper{w->generate_wallpaper()};
    // Paint layout empty space.
    // if (w->screen_state().is_layout) {
    if (has_children(w)) {
        // create bitmask
        detail::Screen_mask mask{detail::find_empty_space(*w)};

        // iterate over bitmask and put wallpapers
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
    for (std::size_t y{w->y()}; y < (w->y() + w->outer_height()); ++y) {
        for (std::size_t x{w->x()}; x < (w->x() + w->outer_width()); ++x) {
            Point p{x, y};
            // if (changes.count(p) == 0 && !w->screen_state().is_layout) {
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
    }
}

// Staged Changes has a tile at this point.
// if (changes_map.count(p) == 1) {
//     Glyph tile{changes_map.at(p)};
//     detail::add_default_attributes(tile, w->brush);
//     // Screen state does not have a tile, or is not equal to
//     // the one that is currently there.
//     // ADD TILE
//     if (screen_iter == end_iter ||
//         (screen_iter != end_iter && screen_iter->second != tile))
//         { if (!child_has_point(*w, p.x, p.y)) {
//             engine_.put(p.x, p.y, tile);
//             w->screen_state().tiles[p] = tile;
//         }
//     }
// } else {
//     // Staged Changes does not have a tile.
//     // But screen state might, put a background tile.
//     // ADD BACKGROUND
//     if (!child_has_point(*w, p.x, p.y)) {
//         engine_.put(p.x, p.y, current_wallpaper);
//     }
// }

// void Paint_buffer::full_paint(
//     Widget* w,
//     const detail::Screen_descriptor& changes_map) {
//     const Glyph& current_wallpaper{w->generate_wallpaper()};
//     for (std::size_t y{w->y()}; y < (w->y() + w->outer_height()); ++y) {
//         for (std::size_t x{w->x()}; x < (w->x() + w->outer_width()); ++x) {
//             Point p{x, y};
//             auto screen_iter = w->screen_state().tiles.find(p);
//             auto end_iter = std::end(w->screen_state().tiles);
//             // Staged Changes has a tile at this point.
//             if (changes_map.count(p) == 1) {
//                 Glyph tile{changes_map.at(p)};
//                 detail::add_default_attributes(tile, w->brush);
//                 // Screen state does not have a tile, or is not equal to
//                 // the one that is currently there.
//                 // ADD TILE
//                 if (screen_iter == end_iter ||
//                     (screen_iter != end_iter && screen_iter->second != tile))
//                     { if (!child_has_point(*w, p.x, p.y)) {
//                         engine_.put(p.x, p.y, tile);
//                         w->screen_state().tiles[p] = tile;
//                     }
//                 }
//             } else {
//                 // Staged Changes does not have a tile.
//                 // But screen state might, put a background tile.
//                 // ADD BACKGROUND
//                 if (!child_has_point(*w, p.x, p.y)) {
//                     engine_.put(p.x, p.y, current_wallpaper);
//                 }
//             }
//         }
//     }
// }

void Paint_buffer::flush_just_enabled(
    Widget* w,
    const detail::Screen_descriptor& changes_map) {
    full_paint(w, changes_map);
}

void Paint_buffer::flush_child_event_happened(
    Widget* w,
    const detail::Screen_descriptor& changes_map) {
    full_paint(w, changes_map);
}

void Paint_buffer::basic_paint(Widget* w,
                               const detail::Screen_descriptor& changes_map) {
    const Glyph& current_background{w->generate_wallpaper()};
    // BACKGROUNDS COVER LEFTOVERS
    std::vector<Point> to_delete;
    for (const auto& point_tile : w->screen_state().tiles) {
        const Point& point{point_tile.first};
        if (changes_map.count(point) == 0) {
            if (!child_has_point(*w, point.x, point.y)) {
                engine_.put(point.x, point.y, current_background);
            }
            to_delete.push_back(point);
        }
    }
    for (const Point& p : to_delete) {
        w->screen_state().tiles.erase(p);
    }
    // NEW TILES PRINTED TO SCREEN
    for (const auto& point_tile : changes_map) {
        const Point& point{point_tile.first};
        Glyph tile{point_tile.second};
        detail::add_default_attributes(tile, w->brush);
        auto screen_iter = w->screen_state().tiles.find(point);
        if (screen_iter != std::end(w->screen_state().tiles) &&
            screen_iter->second == tile) {
            continue;
        } else {
            if (!child_has_point(*w, point.x, point.y)) {
                engine_.put(point.x, point.y, tile);
                w->screen_state().tiles[point] = tile;
            }
        }
    }
}

// TODO implement.
void Paint_buffer::flush_move_resize_event(
    Widget* w,
    const detail::Screen_descriptor& changes_map) {
    basic_paint(w, changes_map);
}

void Paint_buffer::flush(const detail::Staged_changes& changes) {
    std::lock_guard<std::mutex> lock{detail::NCurses_data::ncurses_mtx};
    bool refresh{false};

#if defined(DEBUG_PAINT_BUFFER_CHILD_SIZES)
    debug_child_size(System::head());
#endif

    for (const auto& pair : changes) {
        Widget* w{pair.first};
        const detail::Screen_descriptor& changes_map{pair.second};

        if (detail::is_not_paintable(w)) {
            w->screen_state().tiles.clear();
            continue;
        }

        const Glyph& current_wallpaper{w->generate_wallpaper()};
        Glyph& last_wallpaper{w->screen_state().optimize.wallpaper};

        refresh = true;

        // full_paint(w, changes_map);

        if (wallpaper_changed(current_wallpaper, last_wallpaper)) {
            full_paint(w, changes_map);
        } else {
            basic_paint(w, changes_map);
        }

        // if (wallpaper_changed(current_wallpaper, last_wallpaper)) {
        //     full_paint(w, changes_map);
        // } else if (w->screen_state().optimize.just_enabled) {
        //     flush_just_enabled(w, changes_map);
        //     w->screen_state().optimize.just_enabled = false;
        // } else if (w->screen_state().optimize.child_event) {
        //     flush_child_event_happened(w, changes_map);
        //     w->screen_state().optimize.child_event = false;
        // } else if (w->screen_state().optimize.moved ||
        //            w->screen_state().optimize.resized) {
        //     flush_move_resize_event(w, changes_map);
        //     w->screen_state().optimize.moved = false;
        //     w->screen_state().optimize.resized = false;
        // } else {
        //     basic_paint(w, changes_map);
        // }

        // If background tile has changed or widget just appeared on screen.
        // if (wallpaper_changed(current_background, last_background) ||
        //     w->screen_state().just_appeared ||
        //     w->screen_state().child_event_happened) {
        //     if (w->screen_state().just_appeared) {
        //         w->screen_state().tiles.clear();
        //         w->screen_state().just_appeared = false;
        //     }
        //     for (std::size_t y{w->y()}; y < (w->y() + w->outer_height());
        //     ++y) {
        //         for (std::size_t x{w->x()}; x < (w->x() + w->outer_width());
        //              ++x) {
        //             Point p{x, y};
        //             auto screen_iter = w->screen_state().tiles.find(p);
        //             auto end_iter = std::end(w->screen_state().tiles);
        //             // Staged Changes has a tile at this point.
        //             if (changes_map.count(p) == 1) {
        //                 // Screen state does not have a tile, or is not equal
        //                 to
        //                 // the one that is currently there.
        //                 if (screen_iter == end_iter ||
        //                     (screen_iter != end_iter &&
        //                      screen_iter->second != tile)) {
        //                     if (!child_has_point(*w, p.x, p.y)) {
        //                         Glyph tile{changes_map.at(p)};
        //                         detail::add_default_attributes(tile,
        //                         w->brush); engine_.put(p.x, p.y, tile);
        //                         w->screen_state().tiles[p] = tile;
        //                     }
        //                 }
        //             } else {
        //                 // Staged Changes does not have a tile at this point.
        //                 // But screen state does, then put a background tile.
        //                 if (!child_has_point(*w, p.x, p.y)) {
        //                     engine_.put(p.x, p.y, current_background);
        //                 }
        //             }
        //         }
        //     }
        // } else {
        //     // BACKGROUNDS COVER LEFTOVERS
        //     std::vector<Point> to_delete;
        //     for (const auto& point_tile : w->screen_state().tiles) {
        //         const Point& point{point_tile.first};
        //         if (changes_map.count(point) == 0) {
        //             if (!child_has_point(*w, point.x, point.y)) {
        //                 engine_.put(point.x, point.y, current_background);
        //             }
        //             to_delete.push_back(point);
        //         }
        //     }
        //     for (const Point& p : to_delete) {
        //         w->screen_state().tiles.erase(p);
        //     }
        // }
        // // NEW TILES PRINTED TO SCREEN
        // for (const auto& point_tile : changes_map) {
        //     const Point& point{point_tile.first};
        //     Glyph tile{point_tile.second};
        //     detail::add_default_attributes(tile, w->brush);
        //     auto screen_iter = w->screen_state().tiles.find(point);
        //     if (screen_iter != std::end(w->screen_state().tiles) &&
        //         screen_iter->second == tile) {
        //         continue;
        //     } else {
        //         if (!child_has_point(*w, point.x, point.y)) {
        //             engine_.put(point.x, point.y, tile);
        //             w->screen_state().tiles[point] = tile;
        //         }
        //     }
        // }

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

// void Paint_buffer::flush(const detail::Staged_changes& changes) {
//     std::lock_guard<std::mutex> lock{detail::NCurses_data::ncurses_mtx};
//     bool refresh{false};

//     // Repaint entire screen if requested.
//     if (repaint_all_) {
//         cover_all_with_background();
//     }

//     for (const auto& pair : changes) {
//         Widget* widg{pair.first};
//         const detail::Screen_descriptor& changes_map{
//             pair.second.screen_description};
//         if (detail::is_not_paintable(widg)) {
//             continue;
//         }
//         refresh = true;
//         // REPAINT ENTIRE BACKGROUND IF REQUESTED.
//         if (!repaint_all_ && pair.second.repaint) {
//             cover_with_background(*widg);
//             widg->screen_state().tiles.clear();
//         }
//         if (repaint_all_) {
//             widg->screen_state().tiles.clear();
//         }
//         // BACKGROUNDS COVER LEFTOVERS
//         std::vector<Point> to_delete;
//         for (const auto& point_tile : widg->screen_state().tiles) {
//             const Point& point{point_tile.first};
//             if (changes_map.count(point) == 0) {
//                 Glyph background{widg->find_background_tile()};
//                 if (within_screen(point)) {
//                     engine_.put(point.x, point.y, background);
//                 }
//                 to_delete.push_back(point);
//             }
//         }
//         for (const Point& p : to_delete) {
//             widg->screen_state().tiles.erase(p);
//         }
//         // NEW TILES PRINTED TO SCREEN
//         for (const auto& point_tile : changes_map) {
//             const Point& point{point_tile.first};
//             Glyph tile{point_tile.second};
//             detail::add_default_attributes(tile, widg->brush);
//             auto screen_iter = widg->screen_state().tiles.find(point);
//             if (screen_iter != std::end(widg->screen_state().tiles) &&
//                 screen_iter->second == tile) {
//                 continue;
//             } else {
//                 if (within_screen(point)) {
//                     engine_.put(point.x, point.y, tile);
//                     widg->screen_state().tiles[point] = tile;
//                 }
//             }
//         }
//     }
//     Widget* focus_widg = Focus::focus_widget();
//     if (focus_widg == nullptr) {
//         engine_.hide_cursor();
//     } else {
//         bool cursor_visible{focus_widg->cursor_visible()};
//         engine_.show_cursor(cursor_visible);
//         if (cursor_visible) {
//             std::size_t x = focus_widg->inner_x() + focus_widg->cursor_x();
//             std::size_t y = focus_widg->inner_y() + focus_widg->cursor_y();
//             engine_.move_cursor(x, y);
//         }
//     }
//     if (refresh) {
//         engine_.refresh();
//     }
//     repaint_all_ = false;
// }

// void Paint_buffer::set_repaint_all() {
//     repaint_all_ = true;
// }

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

// void Paint_buffer::resize_width(std::size_t new_width) {
//     // std::lock_guard<Mutex_t> guard{mutex_};
//     width_ = new_width;
// }

// void Paint_buffer::resize_height(std::size_t new_height) {
//     // std::lock_guard<Mutex_t> guard{mutex_};
//     height_ = new_height;
// }

}  // namespace cppurses
