#include <cppurses/painter/paint_buffer.hpp>

#include <cstddef>
#include <mutex>

#include <cppurses/painter/detail/glyph_and_bkgd_bool.hpp>
#include <cppurses/painter/detail/is_not_paintable.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_matrix.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/painter/palette.hpp>
#include <cppurses/system/detail/repaint_all.hpp>
#include <cppurses/system/events/paint_event.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/widget.hpp>

// #include <utility/log.hpp>  //temp

// namespace {
// using namespace cppurses;

// Checks whether global point is within the boundaries of a given widget.
// bool within_widg(const Point& point, const Widget* widg) {
//     // utility::Log l;
//     // l << "within_widg called\n";
//     std::size_t west_boundary{widg->x()};
//     if (point.x < west_boundary) {
//         // l << "Failed at west: point.x: " << point.x
//         //   << " west_boundary: " << west_boundary << std::endl;
//         return false;
//     }
//     std::size_t north_boundary{widg->y()};
//     if (point.y < north_boundary) {
//         // l << "Failed at north: point.y: " << point.y
//         //   << " north_boundary: " << north_boundary << std::endl;
//         return false;
//     }
//     std::size_t east_boundary{west_boundary + widg->width() +
//                               west_border_offset(*widg) +
//                               east_border_offset(*widg)};
//     if (point.x >= east_boundary) {
//         // l << "Failed at east: point.x: " << point.x
//         //   << " east_boundary: " << east_boundary << std::endl;
//         return false;
//     }
//     std::size_t south_boundary{north_boundary + widg->height() +
//                                north_border_offset(*widg) +
//                                south_border_offset(*widg)};
//     if (point.y >= south_boundary) {
//         // l << "Failed at south: point.y: " << point.y
//         //   << " south_boundary: " << south_boundary << std::endl;
//         return false;
//     }
//     // l << "x: " << point.x << " y: " << point.y << "is within widget
//     with:\n";
//     // l << "west_boundary: " << west_boundary << '\n';
//     // l << "east_boundary: " << east_boundary << '\n';
//     // l << "north_boundary: " << north_boundary << '\n';
//     // l << "south_boundary: " << south_boundary << std::endl;
//     return true;
// }

// }  // namespace

namespace cppurses {

Paint_buffer::Paint_buffer() {
    this->update_width();
    this->update_height();
}

// void Paint_buffer::update_diff(const Painter& p) {
//     // Painter.state_ against Screen_state_ - Place new Glyphs in diff.
//     // utility::Log l;
//     // if (p.state().size() > 0) {
//     //     l << "UPDATE_DIFF() CALLED - - - - - - - - - - - - - - - - - - - -
//     -
//     //     "
//     //          "\n";
//     //     l << "- - - - PAINTER.STATE ITERATED, COMPARED TO SCREEN_STATE\n";
//     // }
//     std::lock_guard<Mutex_t> guard{mutex_};
//     if (detail::is_not_paintable(p.widget())) {
//         return;
//     }
//     for (const auto& point_glyph_pair : p.state()) {
//         const Point& point{point_glyph_pair.first};
//         const detail::Glyph_and_bkgd_bool& painter_glyph{
//             point_glyph_pair.second};
//         auto screen_at_point = screen_state_.find(point);
//         // Overwrite existing glyph that is on the screen.
//         // if staged tile is found to already exist in screen_state
//         if (screen_at_point != std::end(screen_state_)) {
//             if (screen_at_point->second != painter_glyph.tile) {
//                 staged_diff_[point] = painter_glyph;
//                 // l << "x " << point.x << " y " << point.y << ' '
//                 //   << painter_glyph.symbol << std::endl;
//             }
//         } else {  // Put glyph in diff if state has nothing there
//             staged_diff_[point] = painter_glyph;
//             // l << "x " << point.x << " y " << point.y << ' '
//             //   << painter_glyph.symbol << std::endl;
//         }
//     }
//     // optimized version? though skipping a copy is great
//     // if (screen_state_.count(point) == 1) {
//     //     staged_diff_[point] = glyph;
//     // }

//     // And another version
//     // if ((screen_state_.count(point) == 1) &&
//     //     (screen_state_[point] == glyph)) {
//     // } else {
//     //     staged_diff_[point] = glyph;
//     // }

//     // if (p.state().size() > 0) {
//     //     l << "- - - - SCREEN_STATE ITERATED, COMPARED TO PAINTER.STATE\n";
//     // }
//     // Screen_state_ iteration - Erase leftover Glyphs with proper
//     background. for (const auto& point_glyph_pair : screen_state_) {
//         const Point& point{point_glyph_pair.first};
//         // const detail::Glyph_and_bkgd_bool& screen_glyph{
//         //     point_glyph_pair.second};
//         if (within_widg(point, p.widget()) && p.state().count(point) == 0) {
//             Glyph background{p.widget()->background_tile()
//                                  ? *p.widget()->background_tile()
//                                  : global_background_tile_};
//             if (p.widget()->brush_alters_background()) {
//                 background = p.add_default_attributes(background);
//             }
//             // if (screen_glyph != background) {
//             staged_diff_[point] = detail::Glyph_and_bkgd_bool{background,
//             true};
//             // l << "x " << point.x << " y " << point.y << ' '
//             //   << background.symbol << std::endl;
//             // }
//         }
//     }
// }

// void Paint_buffer::flush(bool optimize) {
//     std::lock_guard<Mutex_t> guard{mutex_};
//     // utility::Log l;
//     // int log_background_glyph{0};
//     // int log_foreground_glyph{0};
//     // Completely redraw the screen if optimize == false
//     if (!optimize) {  // this'll have to change. call on widgets to update
//     fully
//         // might never be called except at construction and isnt needed there
//         // except maybe palette change?
//         this->repaint_all_backgrounds();
//         for (const auto& point_glyph_pair : screen_state_) {
//             engine_.put(point_glyph_pair.first.x, point_glyph_pair.first.y,
//                         point_glyph_pair.second);
//         }
//     }
//     // Put the current diff to the screen
//     // utility::Log l;
//     for (const auto& point_glyph_pair : staged_diff_) {
//         const Point& point{point_glyph_pair.first};
//         const detail::Glyph_and_bkgd_bool& staged_glyph{
//             point_glyph_pair.second};
//         // l << "x " << point_glyph_pair.first.x << " y "
//         //   << point_glyph_pair.first.y << ' ' <<
//         //   point_glyph_pair.second.symbol
//         //   << std::endl;
//         engine_.put(point.x, point.y, staged_glyph.tile);
//         // if (staged_glyph.is_background) {
//         //     ++log_background_glyph;
//         // }
//         if (!staged_glyph.is_background) {
//             // ++log_foreground_glyph;
//             screen_state_[point] = staged_glyph.tile;
//         }
//     }
//     if (!optimize) {
//         engine_.touch_all();
//     }
//     staged_diff_.clear();
//     // Display Cursor
//     Widget* focus_widg = Focus::focus_widget();
//     if (focus_widg == nullptr) {
//         engine_.hide_cursor();
//     } else {
//         bool cursor_visible{focus_widg->cursor_visible()};
//         engine_.show_cursor(cursor_visible);
//         if (cursor_visible) {
//             std::size_t x = focus_widg->x() + focus_widg->cursor_x();
//             std::size_t y = focus_widg->y() + focus_widg->cursor_y();
//             engine_.move_cursor(x, y);
//         }
//     }
//     engine_.refresh();
//     // l << "Flush called: backgrounds printed: " << log_background_glyph <<
//     // '\n'; l << "              foregrounds printed: " <<
//     log_foreground_glyph
//     //   << std::endl;
//     // if (!optimize) {
//     //     l << "was not optimized." << std::endl;
//     // }
// }

void Paint_buffer::move_cursor(std::size_t x, std::size_t y) {
    std::lock_guard<Mutex_t> guard{mutex_};
    engine_.move_cursor(x, y);
}

// const Glyph& Paint_buffer::at(const Point& point) const {
//     std::lock_guard<Mutex_t> guard{mutex_};
//     auto pos = screen_state_.find(point);
//     if (pos != std::end(screen_state_)) {
//         return pos->second;
//     }
//     return global_background_tile_;
// }

// const Glyph& Paint_buffer::at(std::size_t x, std::size_t y) const {
//     std::lock_guard<Mutex_t> guard{mutex_};
//     return at(Point{x, y});
// }

void Paint_buffer::set_global_background_tile(const Glyph& tile) {
    std::lock_guard<Mutex_t> guard{mutex_};
    global_background_tile_ = tile;
    detail::repaint_all();
    // if (System::head() != nullptr) {
    //     System::send_event(Paint_event{System::head(), true});
    // }
    // for (std::size_t i{0}; i < width_; ++i) {
    //     for (std::size_t j{0}; j < height_; ++j) {
    //         if (screen_state_.count(Point{i, j}) == 0) {
    //             engine_.put(i, j, global_background_tile_);
    //             screen_state_[Point{i, j}] = global_background_tile_;
    //         }
    //     }
    // }
}

Glyph Paint_buffer::get_global_background_tile() const {
    std::lock_guard<Mutex_t> guard{mutex_};
    return global_background_tile_;
}

std::size_t Paint_buffer::update_width() {
    std::lock_guard<Mutex_t> guard{mutex_};
    std::size_t new_width{engine_.screen_width()};
    this->resize_width(new_width);
    return new_width;
}

std::size_t Paint_buffer::update_height() {
    std::lock_guard<Mutex_t> guard{mutex_};
    std::size_t new_height{engine_.screen_height()};
    this->resize_height(new_height);
    return new_height;
}

std::size_t Paint_buffer::screen_width() const {
    std::lock_guard<Mutex_t> guard{mutex_};
    return width_;
}

std::size_t Paint_buffer::screen_height() const {
    std::lock_guard<Mutex_t> guard{mutex_};
    return height_;
}

void Paint_buffer::set_color_definition(Color c, RGB values) {
    std::lock_guard<Mutex_t> guard{mutex_};
    engine_.set_rgb(c, values.red, values.green, values.blue);
}

void Paint_buffer::resize_width(std::size_t new_width) {
    std::lock_guard<Mutex_t> guard{mutex_};
    // if (new_width > width_) {
    // this->paint_background(Point{width_, 0},
    //                        Area{new_width - width_, height_});
    // defer to move/resize event.
    // }
    width_ = new_width;
}

void Paint_buffer::resize_height(std::size_t new_height) {
    std::lock_guard<Mutex_t> guard{mutex_};
    // if (new_height > height_) {
    // this->paint_background(Point{0, height_},
    //                        Area{width_, new_height - height_});
    // }
    height_ = new_height;
}

// remove
// void Paint_buffer::paint_background(Point top_left, Area area) {
// for (std::size_t x{top_left.x}; x < (top_left.x + area.width); ++x) {
//     for (std::size_t y{top_left.y}; y < (top_left.y + area.height); ++y)
//     {
//         staged_diff_[Point{x, y}] = global_background_tile_;
//     }
// }
// }

// void Paint_buffer::repaint_all_backgrounds() {
//     if (System::head() != nullptr) {
//         System::send_event(Paint_event(System::head(), true));
//     }
// loop over width then height calling to engine to move then put
// for (std::size_t i{0}; i < width_; ++i) {
//     for (std::size_t j{0}; j < height_; ++j) {
//         engine_.put(i, j, global_background_tile_);
//         screen_state_[Point{i, j}] = global_background_tile_;
//     }
// }
// }

}  // namespace cppurses
