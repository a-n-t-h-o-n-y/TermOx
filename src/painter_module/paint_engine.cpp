#include "painter_module/paint_engine.hpp"
#include "painter_module/glyph.hpp"
#include <cstddef>

namespace twf {

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
    this->move(buffer_.cursor_position.x, buffer_.cursor_position.y);
    this->refresh();
}

}  // namespace twf
