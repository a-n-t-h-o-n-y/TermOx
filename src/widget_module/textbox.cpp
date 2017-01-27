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
    this->paint_engine().move(this->x() + this->cursor_x(),
                              this->y() + this->cursor_y());
    return Widget::paint_event(event);
}

bool Textbox::key_press_event(const Key_event& event) {
    Painter p{this};
    // Backspace
    if (event.key_code() == Key::Backspace) {
        if (this->cursor_x() == 0 && this->cursor_y() != 0) {
            if (contents_.back().str() == "\n") {
                contents_.pop_back();
                --cursor_index_;
                auto pos = this->cursor_position_2d();
                p.move(pos[0], pos[1]);
                this->paint_engine().move(this->x() + this->cursor_x(),
                                          this->y() + this->cursor_y());
                return true;
            } else {
                p.move(this->geometry().width() - 1, this->cursor_y() - 1);
                this->paint_engine().clear(this->x() + this->cursor_x(),
                                           this->y() + this->cursor_y());
            }
        } else if (this->cursor_x() == 0 && this->cursor_y() == 0) {
            return true;
        } else {
            p.move(this->cursor_x() - 1, this->cursor_y());
            this->paint_engine().clear(this->x() + this->cursor_x(),
                                       this->y() + this->cursor_y());
        }
        contents_.pop_back();
        --cursor_index_;
        // Enter
    } else if (event.key_code() == Key::Enter) {
        contents_.append("\n");
        ++cursor_index_;
        p.put("\n");
        // Character
    } else if (event.text().size() != 0) {
        contents_.append(event.text());
        ++cursor_index_;
        p.put(event.text());
    }
    return true;
}

std::array<std::size_t, 2> Textbox::cursor_position_2d() {
    std::array<std::size_t, 2> position{{0, 0}};  // 0 = x, 1 = y
    for (std::size_t i{0}; i < cursor_index_;
         ++i) {  // once scrol, start at offset
        if (contents_.at(i).str() ==
                "\n" ||  // check for index going out of bounds
            position.at(0) + 1 == this->geometry().width()) {
            ++position.at(1);
            position.at(0) = 0;
        } else {
            ++position.at(0);
        }
    }
    return position;
}

std::size_t Textbox::index_from_position(std::size_t x, std::size_t y) {
    return 0;
}

}  // namespace twf
