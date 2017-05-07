#include <painter_module/paint_engine.hpp>

namespace twf {

void Paint_engine::put(unsigned x, unsigned y, const Glyph& g) {
    buffer_.stage(x, y, g);
}

// probably no need for this now.
// void Paint_engine::clear(unsigned x, unsigned y) {
//     buffer_.stage(x, y, " ");
//     buffer_.commit(x, y);
//     this->move(x, y);
//     this->put_glyph(" ");
//     this->move(x, y);
//     this->refresh();
// }

void Paint_engine::flush(bool optimize) {
    for (int j{0}; j < buffer_.height(); ++j) {
        for (int i{0}; i < buffer_.width(); ++i) {
            if (buffer_.commit(i, j) || !optimize) {
                this->move(i, j);
                this->put_glyph(buffer_.at(i, j));
            }
        }
    }
    if (!optimize) {
        this->touch_all();
    }
    this->move(buffer_.cursor_position.x, buffer_.cursor_position.y);
    this->refresh();
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

}  // namespace twf
