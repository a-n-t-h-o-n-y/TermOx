#include <cppurses/painter/paint_buffer.hpp>

#include <cstddef>
#include <mutex>

#include <cppurses/painter/detail/add_default_attributes.hpp>
#include <cppurses/painter/detail/is_not_paintable.hpp>
#include <cppurses/painter/detail/ncurses_data.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_matrix.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/painter/palette.hpp>
#include <cppurses/painter/rgb.hpp>
#include <cppurses/system/detail/find_widget_at.hpp>
// #include <cppurses/system/detail/repaint_all.hpp>
#include <cppurses/system/events/paint_event.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/widget.hpp>

namespace {
using namespace cppurses;
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

bool background_changed(const Glyph& first, const Glyph& second) {
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

void Paint_buffer::flush_background_changed(
    Widget* w,
    const detail::Screen_descriptor& changes_map) {
    const Glyph& current_background{w->find_background_tile()};
    for (std::size_t y{w->y()}; y < (w->y() + w->outer_height()); ++y) {
        for (std::size_t x{w->x()}; x < (w->x() + w->outer_width()); ++x) {
            Point p{x, y};
            auto screen_iter = w->screen_state().tiles.find(p);
            auto end_iter = std::end(w->screen_state().tiles);
            // Staged Changes has a tile at this point.
            if (changes_map.count(p) == 1) {
                Glyph tile{changes_map.at(p)};
                detail::add_default_attributes(tile, w->brush);
                // Screen state does not have a tile, or is not equal to
                // the one that is currently there.
                // ADD TILE
                if (screen_iter == end_iter ||
                    (screen_iter != end_iter && screen_iter->second != tile)) {
                    if (!child_has_point(*w, p.x, p.y)) {
                        engine_.put(p.x, p.y, tile);
                        w->screen_state().tiles[p] = tile;
                    }
                }
            } else {
                // Staged Changes does not have a tile.
                // But screen state might, put a background tile.
                // ADD BACKGROUND
                if (!child_has_point(*w, p.x, p.y)) {
                    engine_.put(p.x, p.y, current_background);
                }
            }
        }
    }
}

void Paint_buffer::flush_just_appeared(
    Widget* w,
    const detail::Screen_descriptor& changes_map) {
    flush_background_changed(w, changes_map);
}

void Paint_buffer::flush_child_event_happened(
    Widget* w,
    const detail::Screen_descriptor& changes_map) {
    flush_background_changed(w, changes_map);
}

void Paint_buffer::flush_minimal(Widget* w,
                                 const detail::Screen_descriptor& changes_map) {
    const Glyph& current_background{w->find_background_tile()};
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
    flush_minimal(w, changes_map);
}

void Paint_buffer::flush(const detail::Staged_changes& changes) {
    std::lock_guard<std::mutex> lock{detail::NCurses_data::ncurses_mtx};
    bool refresh{false};

    for (const auto& pair : changes) {
        Widget* w{pair.first};
        const detail::Screen_descriptor& changes_map{pair.second};

        if (detail::is_not_paintable(w)) {
            w->screen_state().tiles.clear();
            continue;
        }

        const Glyph& current_background{w->find_background_tile()};
        Glyph& last_background{w->screen_state().background_tile};

        refresh = true;

        if (background_changed(current_background, last_background)) {
            flush_background_changed(w, changes_map);
        } else if (w->screen_state().just_appeared) {
            flush_just_appeared(w, changes_map);
        } else if (w->screen_state().child_event_happened) {
            flush_child_event_happened(w, changes_map);
        } else if (w->screen_state().move_happened ||
                   w->screen_state().resize_happened) {
            flush_move_resize_event(w, changes_map);
        } else {
            flush_minimal(w, changes_map);
        }

        // If background tile has changed or widget just appeared on screen.
        // if (background_changed(current_background, last_background) ||
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

        last_background = current_background;
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
    std::size_t new_width{engine_.screen_width()};
    this->resize_width(new_width);
    return new_width;
}

std::size_t Paint_buffer::update_height() {
    // std::lock_guard<std::mutex> guard{detail::NCurses_data::ncurses_mtx};
    std::size_t new_height{engine_.screen_height()};
    this->resize_height(new_height);
    return new_height;
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

void Paint_buffer::resize_width(std::size_t new_width) {
    // std::lock_guard<Mutex_t> guard{mutex_};
    width_ = new_width;
}

void Paint_buffer::resize_height(std::size_t new_height) {
    // std::lock_guard<Mutex_t> guard{mutex_};
    height_ = new_height;
}

}  // namespace cppurses
