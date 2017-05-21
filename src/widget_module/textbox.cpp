#include "widget_module/widgets/textbox.hpp"
#include "painter_module/glyph_string.hpp"
#include "system_module/events/key_event.hpp"
#include "system_module/events/mouse_event.hpp"
#include "system_module/key.hpp"
#include "widget_module/focus_policy.hpp"
#include <iterator>
#include <utility>
#include <cstddef>

namespace twf {

Textbox::Textbox(Glyph_string contents) : Textbox_core{std::move(contents)} {
    this->set_focus_policy(Focus_policy::Strong);
}

void Textbox::enable_scroll_wheel(bool enable) {
    scroll_wheel_ = enable;
}

void Textbox::disable_scroll_wheel(bool disable) {
    scroll_wheel_ = !disable;
}

bool Textbox::does_scroll_wheel() const {
    return scroll_wheel_;
}

void Textbox::set_wheel_speed(std::size_t lines) {
    this->set_wheel_speed_up(lines);
    this->set_wheel_speed_down(lines);
}

void Textbox::set_wheel_speed_up(std::size_t lines) {
    scroll_speed_up_ = lines;
}

void Textbox::set_wheel_speed_down(std::size_t lines) {
    scroll_speed_down_ = lines;
}

bool Textbox::key_press_event(const Key_event& event) {
    switch (event.key_code()) {
        case Key::Backspace: {
            auto original_index = this->string_index(this->cursor_coordinate());
            if (original_index == 0) {
                return true;
            }
            this->erase(original_index - 1, 1);
            auto new_index = this->string_index(this->cursor_coordinate()) + 1;
            this->cursor_left(new_index - original_index);
        }
        // this->cursor_left();
        // this->erase(this->string_index(this->cursor_coordinate()), 1);

        // this is duplicate of arrow left, put this away maybe into
        // textbox_core, it might not be so bad to have scrolling within the
        // cursor functions in textbox_core, it'd be nice actually so you
        // don't have logic for the same thing across two different
        // functions.
        // if (this->cursor_x() == 0 && this->cursor_y() == 0 &&
        //     this->string_index(0, 0) != 0) {
        //     this->scroll_up();
        //     this->set_cursor_coordinates(0, this->cursor_y() + 1);
        // }

        // if (this->cursor_x() == 0 && this->cursor_y() != 0) {
        //     if (contents_.at(cursor_index_ - 1).str() == "\n") {
        //         --cursor_index_;
        //         contents_.erase(std::begin(contents_) +
        //         cursor_index_);
        //         auto pos = this->position_from_index(cursor_index_);
        //         this->move_cursor(pos.x, pos.y);
        //         this->update();
        //         return true;
        //     }
        //     this->move_cursor(this->width() - 1, this->cursor_y() -
        //     1);
        // } else if (cursor_index_ == 0) {
        //     return true;
        // }
        // else if (this->cursor_x() == 0 && this->cursor_y() == 0) {
        //     this->scroll_up();
        // }
        // else {
        //     this->move_cursor(this->cursor_x() - 1, this->cursor_y());
        // }
        // this->set_cursor_index(cursor_index_ - 1);
        // contents_.erase(std::begin(contents_) + cursor_index_);
        break;

        case Key::Enter:
            this->insert("\n", this->string_index(this->cursor_coordinate()));
            // if (cursor_index_ == contents_.size()) {
            //     contents_.append("\n");
            // } else {
            //     contents_.insert(std::begin(contents_) + cursor_index_,
            //     "\n");
            // }
            if (this->cursor_y() == this->height() - 1) {
                this->scroll_down();
            } else {
                this->cursor_right();
            }
            // if (this->height() != 1) {  // otherwise index moved twice.
            //     this->set_cursor_index(cursor_index_ + 1);
            // }
            break;

        case Key::Tab:
            // Insert tab character, in Widget::move or somewhere you will have
            // the logic for spacing the tab on the screen.
            break;

        case Key::Arrow_right:
            // if (cursor_index_ == contents_.size()) {
            //     return true;
            // }
            // if (this->cursor_y() == this->height() - 1) {
            //     if (this->contents_.at(cursor_index_) == '\n' ||
            //         this->cursor_x() == this->width() - 1) {
            //         this->scroll_down();
            //     }
            // }
            // this->set_cursor_index(cursor_index_ + 1);
            // {
            //     auto pos = position_from_index(cursor_index_);
            //     this->move_cursor(pos.x, pos.y);
            // }
            this->cursor_right();
            break;

        case Key::Arrow_left:
            // if (cursor_index_ != 0) {
            //     if (this->cursor_y() == 0 && this->cursor_x() == 0) {
            //         this->scroll_up();
            //     }
            //     this->set_cursor_index(cursor_index_ - 1);
            //     auto pos = position_from_index(cursor_index_);
            //     this->move_cursor(pos.x, pos.y);
            // }
            // if (this->cursor_x() == 0 && this->cursor_y() == 0 &&
            //     this->string_index(0, 0) != 0) {
            //     this->scroll_up();
            //     this->set_cursor_coordinates(0, this->cursor_y() + 1);
            // }
            this->cursor_left();
            break;

        case Key::Arrow_up:
            this->cursor_up();
            break;

        case Key::Arrow_down:
            this->cursor_down();
            break;

        default:
            if (!event.text().empty()) {
                this->insert(event.text(),
                             this->string_index(this->cursor_coordinate()));

                // if (cursor_index_ == contents_.size()) {
                //     contents_.append(event.text());
                // } else {
                //     contents_.insert(std::begin(contents_) + cursor_index_,
                //                      event.text());
                // }
                if (this->cursor_y() == this->height() - 1 &&
                    this->cursor_x() == this->width() - 1) {
                    this->scroll_down();
                } else {
                    this->cursor_right();
                    // this->set_cursor_index(cursor_index_ + 1);
                }
            }
    }
    this->update();
    return true;
}

bool Textbox::mouse_press_event(const Mouse_event& event) {
    if (event.button() == Mouse_button::Left) {
        this->set_cursor_coordinates(event.local_x(), event.local_y());
        // cursor_index_ = index_from_position(event.local_x(),
        // event.local_y());
        // auto pos = position_from_index(cursor_index_);
        // this->move_cursor(pos.x, pos.y);
    } else if (event.button() == Mouse_button::ScrollUp) {
        if (scroll_wheel_) {
            this->scroll_up(scroll_speed_up_);
        }
    } else if (event.button() == Mouse_button::ScrollDown) {
        if (scroll_wheel_) {
            this->scroll_down(scroll_speed_down_);
        }
    }
    this->update();
    return true;
}

}  // namespace twf
