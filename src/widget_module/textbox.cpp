#include "widget_module/widgets/textbox.hpp"
#include "painter_module/glyph_string.hpp"
#include "system_module/events/key_event.hpp"
#include "system_module/events/mouse_event.hpp"
#include "system_module/key.hpp"
#include "widget_module/focus_policy.hpp"
#include <iterator>

namespace twf {

Textbox::Textbox(const Glyph_string& contents) : Textbox_core{contents} {
    this->set_focus_policy(Focus_policy::Strong);
    this->enable_cursor();
}

bool Textbox::key_press_event(const Key_event& event) {
    // Seperate out the insertion of chars from scrolling and cursor position.
    // They are all related, insertion/deletion causes cursor movement, cursor
    // movement off the screen causes scrolling.

    switch (event.key_code()) {
        case Key::Backspace:
            if (this->cursor_x() == 0 && this->cursor_y() != 0) {
                if (contents_.at(cursor_index_ - 1).str() == "\n") {
                    --cursor_index_;
                    contents_.erase(std::begin(contents_) + cursor_index_);
                    auto pos = this->position_from_index(cursor_index_);
                    this->move_cursor(pos.x, pos.y);
                    this->update();
                    return true;
                }
                this->move_cursor(this->width() - 1, this->cursor_y() - 1);
            } else if (cursor_index_ == 0) {
                return true;
            } else if (this->cursor_x() == 0 && this->cursor_y() == 0) {
                this->scroll_up();
            } else {
                this->move_cursor(this->cursor_x() - 1, this->cursor_y());
            }
            this->set_cursor_index(cursor_index_ - 1);
            contents_.erase(std::begin(contents_) + cursor_index_);
            this->update();
            break;

        case Key::Enter:
            // this is same as forz char, can you set char to '\n' for enter
            // key?
            if (cursor_index_ == contents_.size()) {
                contents_.append("\n");
            } else {
                contents_.insert(std::begin(contents_) + cursor_index_, "\n");
            }
            if (this->cursor_y() == this->height() - 1) {
                this->scroll_down();
            }
            if (this->height() != 1) {  // otherwise index moved twice.
                this->set_cursor_index(cursor_index_ + 1);
            }
            this->update();
            break;

        case Key::Tab:
            // Insert tab character, in Widget::move or somewhere you will have
            // the logic for spacing the tab on the screen.
            break;

        case Key::Arrow_right:
            if (cursor_index_ == contents_.size()) {
                return true;
            }
            if (this->cursor_y() == this->height() - 1) {
                if (this->contents_.at(cursor_index_) == '\n' ||
                    this->cursor_x() == this->width() - 1) {
                    this->scroll_down();
                }
            }
            this->set_cursor_index(cursor_index_ + 1);
            {
                auto pos = position_from_index(cursor_index_);
                this->move_cursor(pos.x, pos.y);
            }
            break;

        case Key::Arrow_left:
            if (cursor_index_ != 0) {
                if (this->cursor_y() == 0 && this->cursor_x() == 0) {
                    this->scroll_up();
                }
                this->set_cursor_index(cursor_index_ - 1);
                auto pos = position_from_index(cursor_index_);
                this->move_cursor(pos.x, pos.y);
            }
            break;

        case Key::Arrow_up:
            if (this->cursor_y() == 0) {
                this->scroll_up();
            }
            this->cursor_up();
            break;

        case Key::Arrow_down:
            if (this->cursor_y() == this->height() - 1 &&
                this->position_from_index(lower_bound_).y !=
                    this->height() - 1) {
                this->scroll_down();  // should to above checks be within
                                      // scroll_down? probably.
            }
            this->cursor_down();
            break;

        default:
            if (!event.text().empty()) {
                if (cursor_index_ == contents_.size()) {
                    contents_.append(event.text());
                } else {
                    contents_.insert(std::begin(contents_) + cursor_index_,
                                     event.text());
                }
                if (this->cursor_y() == this->height() - 1 &&
                    this->cursor_x() == this->width() - 1) {
                    this->scroll_down();
                } else {
                    this->set_cursor_index(cursor_index_ + 1);
                }
                this->update();
            }
    }
    return true;
}

bool Textbox::mouse_press_event(const Mouse_event& event) {
    if (event.button() == Mouse_button::Left) {
        cursor_index_ = index_from_position(event.local_x(), event.local_y());
        auto pos = position_from_index(cursor_index_);
        this->move_cursor(pos.x, pos.y);
    } else if (event.button() == Mouse_button::ScrollUp) {
        this->scroll_up();
    } else if (event.button() == Mouse_button::ScrollDown) {
        this->scroll_down();
    }
    return true;
}

}  // namespace twf
