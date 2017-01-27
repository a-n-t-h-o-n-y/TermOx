#include <widget_module/widgets/textbox.hpp>

#include <painter_module/painter.hpp>
#include <system_module/key.hpp>
#include <system_module/events/paint_event.hpp>
#include <system_module/events/key_event.hpp>
#include <system_module/system.hpp>

namespace twf {

bool Textbox::paint_event(const Paint_event& event) {
    Painter p{this};
    p.put_at(0, 0, contents_, false);
    auto pos = this->cursor_position_2d();
    p.move(pos[0], pos[1]);
    return Widget::paint_event(event);
}

bool Textbox::key_press_event(const Key_event& event) {
    Painter p{this};
    // Backspace
    if (event.key_code() == Key::Backspace) {
        if (this->cursor_x() == 0 && this->cursor_y() != 0) {
            if (contents_.at(cursor_index_ - 1).str() == "\n") {
                --cursor_index_;
                contents_.erase(std::begin(contents_) + cursor_index_);
                auto pos = this->cursor_position_2d();
                p.move(pos[0], pos[1]);
                this->update();
                return true;
            } else {
                p.move(this->geometry().width() - 1, this->cursor_y() - 1);
                p.put(" ", false);
            }
        } else if (this->cursor_x() == 0 && this->cursor_y() == 0) {
            return true;
        } else {
            p.move(this->cursor_x() - 1, this->cursor_y());
            p.put(" ", false);
        }
        --cursor_index_;
        contents_.erase(std::begin(contents_) + cursor_index_);
        // Enter
    } else if (event.key_code() == Key::Enter) {
        if (cursor_index_ == contents_.size()) {
            contents_.append("\n");
        } else {
            contents_.insert(std::begin(contents_) + cursor_index_, "\n");
            this->update();
        }
        ++cursor_index_;
        p.put("\n");
        // Character
    } else if (event.text().size() != 0) {
        if (cursor_index_ == contents_.size()) {
            contents_.append(event.text());
        } else {
            contents_.insert(std::begin(contents_) + cursor_index_,
                             event.text());
            this->update();
        }
        ++cursor_index_;
        p.put(event.text());
    } else if (event.key_code() == Key::Arrow_right) {
        if (cursor_index_ != contents_.size()) {
            ++cursor_index_;
            auto pos = cursor_position_2d();
            p.move(pos[0], pos[1]);
        }
    } else if (event.key_code() == Key::Arrow_left) {
        if (cursor_index_ != 0) {
            --cursor_index_;
            auto pos = cursor_position_2d();
            p.move(pos[0], pos[1]);
        }
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
