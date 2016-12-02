#include <mcurses/painter_module/paint_engine.hpp>

namespace mcurses {

void Paint_engine::put(unsigned x, unsigned y, const Glyph& g) {
    buffer_.stage(x, y, g);
}

void Paint_engine::flush() {
    for (int j{0}; j < buffer_.height(); ++j) {
        for (int i{0}; i < buffer_.width(); ++i) {
            if (buffer_.commit(i, j)) {
                this->move(i, j);
                this->put_glyph(buffer_.at(i, j));
            }
        }
    }
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
    this->put_string(g.str());
    this->clear_attributes();
}

}  // namespace mcurses
