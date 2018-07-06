#include <cppurses/painter/paint_buffer.hpp>

#include <cstddef>
#include <mutex>

#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_matrix.hpp>
#include <cppurses/painter/palette.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

Paint_buffer::Paint_buffer() {
    this->update_width();
    this->update_height();
}

void Paint_buffer::stage(std::size_t x, std::size_t y, const Glyph& glyph) {
    std::lock_guard<Mutex_t> guard{mutex_};
    if (y >= staging_area_.height() || x >= staging_area_.width()) {
        return;
    }
    if (staging_area_(x, y) != glyph) {
        staging_area_(x, y) = glyph;
    }
}

void Paint_buffer::flush(bool optimize) {
    std::lock_guard<Mutex_t> guard{mutex_};
    // instead of this double loop, iterate over list.
    for (std::size_t j{0}; j < staging_area_.height(); ++j) {
        for (std::size_t i{0}; i < staging_area_.width(); ++i) {
            if (this->commit(i, j) || !optimize) {
                engine_.move(i, j);
                engine_.put_glyph(this->at(i, j)); // change at to staged_at
            }
        }
    }
    // Forces redraw of the entire screen.
    if (!optimize) {
        engine_.touch_all();
    }
    // Set cursor
    auto* focus_widg = Focus::focus_widget();
    if (focus_widg != nullptr) {
        engine_.show_cursor(focus_widg->cursor_visible());
        if (focus_widg->cursor_visible()) {
            auto x = focus_widg->x() + focus_widg->cursor_x();
            auto y = focus_widg->y() + focus_widg->cursor_y();
            engine_.move(x, y);
        }
    } else {
        engine_.hide_cursor();
    }
    engine_.refresh();
}

void Paint_buffer::move(std::size_t x, std::size_t y) {
    std::lock_guard<Mutex_t> guard{mutex_};
    engine_.move(x, y);
}

const Glyph& Paint_buffer::at(std::size_t x, std::size_t y) const {
    std::lock_guard<Mutex_t> guard{mutex_};
    return backing_store_.at(x, y);
}

std::size_t Paint_buffer::update_width() {
    std::lock_guard<Mutex_t> guard{mutex_};
    std::size_t width = engine_.screen_width();
    this->resize(width, staging_area_.height());
    return width;
}

std::size_t Paint_buffer::update_height() {
    std::lock_guard<Mutex_t> guard{mutex_};
    std::size_t height = engine_.screen_height();
    this->resize(staging_area_.width(), height);
    return height;
}

void Paint_buffer::set_color(Color c, RGB values) {
    std::lock_guard<Mutex_t> guard{mutex_};
    engine_.set_rgb(c, values.red, values.green, values.blue);
}

void Paint_buffer::resize(std::size_t width, std::size_t height) {
    std::lock_guard<Mutex_t> guard{mutex_};
    backing_store_.resize(width, height);
    staging_area_.resize(width, height);
}

bool Paint_buffer::commit(std::size_t x, std::size_t y) {
    std::lock_guard<Mutex_t> guard{mutex_};
    if (staging_area_(x, y) == backing_store_(x, y)) {
        return false;
    }
    backing_store_(x, y) = staging_area_(x, y);
    return true;
}

}  // namespace cppurses
