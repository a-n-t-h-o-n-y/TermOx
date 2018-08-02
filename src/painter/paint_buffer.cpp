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
#include <cppurses/system/detail/repaint_all.hpp>
#include <cppurses/system/events/paint_event.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/widget.hpp>

namespace {
using namespace cppurses;
bool child_has_point(const Widget& w, std::size_t x, std::size_t y) {
    const std::vector<std::unique_ptr<Widget>>& children{w.children()};
    for (const std::unique_ptr<Widget>& child : children) {
        if (x >= child->x() && x < (child->inner_x() + child->outer_width()) &&
            y >= child->y() && y < (child->inner_y() + child->outer_height())) {
            return true;
        }
    }
    return false;
}
}  // namespace

namespace cppurses {

Paint_buffer::Paint_buffer() {
    this->update_width();
    this->update_height();
}

bool Paint_buffer::within_screen(const Point& p) {
    if (p.x >= this->screen_width() || p.y >= this->screen_height()) {
        return false;
    }
    return true;
}

void Paint_buffer::cover_all_with_background() {
    for (std::size_t y{0}; y < (System::max_height()); ++y) {
        for (std::size_t x{0}; x < (System::max_width()); ++x) {
            Widget* w{detail::find_widget_at(x, y)};
            // TODO throw here if == nullptr, for debugging.
            if (w != nullptr) {
                Glyph background{w->find_background_tile()};
                engine_.put(x, y, background);
            }
        }
    }
}

void Paint_buffer::cover_with_background(Widget& w) {
    Glyph background{w.find_background_tile()};
    for (std::size_t y{w.y()}; y < (w.y() + w.outer_height()); ++y) {
        for (std::size_t x{w.x()}; x < (w.x() + w.outer_width()); ++x) {
            if (!child_has_point(w, x, y)) {
                engine_.put(x, y, background);
            }
        }
    }
}

void Paint_buffer::flush(const detail::Staged_changes& changes) {
    std::lock_guard<std::mutex> lock{detail::NCurses_data::ncurses_mtx};
    bool refresh{false};

    // Repaint entire screen if requested.
    if (repaint_all_) {
        cover_all_with_background();
    }

    for (const auto& pair : changes) {
        Widget* widg{pair.first};
        const detail::Screen_descriptor& changes_map{
            pair.second.screen_description};
        if (detail::is_not_paintable(widg)) {
            continue;
        }
        refresh = true;
        // REPAINT ENTIRE BACKGROUND IF REQUESTED.
        if (!repaint_all_ && pair.second.repaint) {
            cover_with_background(*widg);
            widg->screen_state().tiles.clear();
        }
        if (repaint_all_) {
            widg->screen_state().tiles.clear();
        }
        // BACKGROUNDS COVER LEFTOVERS
        std::vector<Point> to_delete;
        for (const auto& point_tile : widg->screen_state().tiles) {
            const Point& point{point_tile.first};
            if (changes_map.count(point) == 0) {
                Glyph background{widg->find_background_tile()};
                if (within_screen(point)) {
                    engine_.put(point.x, point.y, background);
                }
                to_delete.push_back(point);
            }
        }
        for (const Point& p : to_delete) {
            widg->screen_state().tiles.erase(p);
        }
        // NEW TILES PRINTED TO SCREEN
        for (const auto& point_tile : changes_map) {
            const Point& point{point_tile.first};
            Glyph tile{point_tile.second};
            detail::add_default_attributes(tile, widg->brush);
            auto screen_iter = widg->screen_state().tiles.find(point);
            if (screen_iter != std::end(widg->screen_state().tiles) &&
                screen_iter->second == tile) {
                continue;
            } else {
                if (within_screen(point)) {
                    engine_.put(point.x, point.y, tile);
                    widg->screen_state().tiles[point] = tile;
                }
            }
        }
    }
    Widget* focus_widg = Focus::focus_widget();
    if (focus_widg == nullptr) {
        engine_.hide_cursor();
    } else {
        bool cursor_visible{focus_widg->cursor_visible()};
        engine_.show_cursor(cursor_visible);
        if (cursor_visible) {
            std::size_t x = focus_widg->inner_x() + focus_widg->cursor_x();
            std::size_t y = focus_widg->inner_y() + focus_widg->cursor_y();
            engine_.move_cursor(x, y);
        }
    }
    if (refresh) {
        engine_.refresh();
    }
    repaint_all_ = false;
}

void Paint_buffer::set_repaint_all() {
    repaint_all_ = true;
}

void Paint_buffer::move_cursor(std::size_t x, std::size_t y) {
    // std::lock_guard<Mutex_t> guard{mutex_};
    engine_.move_cursor(x, y);
}

void Paint_buffer::set_global_background_tile(const Glyph& tile) {
    // std::lock_guard<Mutex_t> guard{mutex_};
    global_background_tile_ = tile;
    detail::repaint_all();
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
