#include <widget_module/widgets/textbox.hpp>

#include <painter_module/painter.hpp>
#include <system_module/key.hpp>
#include <system_module/events/paint_event.hpp>
#include <system_module/events/key_event.hpp>
#include <system_module/system.hpp>

namespace twf {

bool Textbox::paint_event(const Paint_event& event) {
    Painter p{this};
    p.move(0, 0);
    p.put(contents_);
    return Widget::paint_event(event);
}

bool Textbox::key_press_event(const Key_event& event) {
    Painter p{this};
    if (event.key_code() == Key::Backspace) {
        if (this->cursor_x() == 0 && this->cursor_y() != 0) {
            if (contents_.back().str() == "\n") {
                contents_.pop_back();
                this->update();
                return Widget::key_press_event(event);
            } else {
                p.move(this->geometry().width() - 1, this->cursor_y() - 1);
            }
        } else if (this->cursor_x() == 0 && this->cursor_y() == 0) {
            return Widget::key_press_event(event);
        } else {
            p.move(this->cursor_x() - 1, this->cursor_y());
        }
        contents_.pop_back();
        this->paint_engine().clear(this->x() + this->cursor_x(),
                                   this->y() + this->cursor_y());
    } else if (event.key_code() == Key::Enter) {
        contents_.append("\n");
        p.put("\n");
    } else if (event.text().size() != 0) {
        contents_.append(event.text());
        p.put(event.text());
    }
    return Widget::key_press_event(event);
}

}  // namespace twf
