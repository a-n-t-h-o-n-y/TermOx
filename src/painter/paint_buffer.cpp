#include <cppurses/painter/paint_buffer.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_matrix.hpp>
#include <cppurses/painter/palette.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/widget.hpp>

#include <cstddef>

namespace cppurses {

Paint_buffer::Paint_buffer() {
    this->update_width();
    this->update_height();
}

void Paint_buffer::stage(std::size_t x, std::size_t y, const Glyph& glyph) {
    if (y >= staging_area_.height() || x >= staging_area_.width()) {
        return;
    }
    if (staging_area_(x, y) != glyph) {
        staging_area_(x, y) = glyph;
    }
}

void Paint_buffer::flush(bool optimize) {
    for (std::size_t j{0}; j < staging_area_.height(); ++j) {
        for (std::size_t i{0}; i < staging_area_.width(); ++i) {
            if (this->commit(i, j) || !optimize) {
                engine_.move(i, j);
                engine_.put_glyph(this->at(i, j));
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
    engine_.move(x, y);
}

const Glyph& Paint_buffer::at(std::size_t x, std::size_t y) const {
    return backing_store_.at(x, y);
}

std::size_t Paint_buffer::update_width() {
    std::size_t width = engine_.screen_width();
    this->resize(width, staging_area_.height());
    return width;
}

std::size_t Paint_buffer::update_height() {
    std::size_t height = engine_.screen_height();
    this->resize(staging_area_.width(), height);
    return height;
}

void Paint_buffer::set_color(Color c, RGB values) {
    engine_.set_rgb(c, values.red, values.green, values.blue);
}

void Paint_buffer::resize(std::size_t x, std::size_t y) {
    backing_store_.resize(x, y);
    staging_area_.resize(x, y);
}

bool Paint_buffer::commit(std::size_t x, std::size_t y) {
    if (staging_area_(x, y) == backing_store_(x, y)) {
        return false;
    }
    backing_store_(x, y) = staging_area_(x, y);
    return true;
}

}  // namespace cppurses
