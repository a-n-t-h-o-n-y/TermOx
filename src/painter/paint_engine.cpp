#include "painter/paint_engine.hpp"
#include "painter/glyph.hpp"
#include "system/system.hpp"
#include "system/focus.hpp"
#include "widget/widget.hpp"
#include <cstddef>

namespace cppurses {

void Paint_engine::put(std::size_t x, std::size_t y, const Glyph& g) {
    buffer_.stage(x, y, g);
}

void Paint_engine::put_glyph(const Glyph& g) {
    for (const Attribute& attr : g.brush().attributes()) {
        this->set_attribute(attr);
    }
    if (g.brush().background_color()) {
        this->set_background_color(*g.brush().background_color());
    }
    if (g.brush().foreground_color()) {
        this->set_foreground_color(*g.brush().foreground_color());
    }
    this->put_string(g.c_str());
    this->clear_attributes();
}

void Paint_engine::flush(bool optimize) {
    for (std::size_t j{0}; j < buffer_.height(); ++j) {
        for (std::size_t i{0}; i < buffer_.width(); ++i) {
            if (buffer_.commit(i, j) || !optimize) {
                this->move(i, j);
                this->put_glyph(buffer_.at(i, j));
            }
        }
    }
    // Forces redraw of the entire screen.
    if (!optimize) {
        this->touch_all();
    }
    // Set cursor
    auto* focus_widg = Focus::focus_widget();
    if (focus_widg != nullptr) {
        this->show_cursor(focus_widg->cursor());
        if (focus_widg->cursor()) {
            auto x = focus_widg->x() + focus_widg->cursor_x() +
                     west_border_offset(focus_widg->border());
            auto y = focus_widg->y() + focus_widg->cursor_y() +
                     north_border_offset(focus_widg->border());
            this->move(x, y);
        }
    } else {
        this->hide_cursor();
    }
    this->refresh();
}

}  // namespace cppurses
