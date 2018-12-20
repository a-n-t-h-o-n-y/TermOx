#include <cppurses/widget/widgets/text_display.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string>
#include <utility>

#include <signals/signal.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/widget/point.hpp>

namespace cppurses {

// TODO why is this update_display here and not in paint_event? Must be a
// reason? It'd be more efficient the other way.
void Text_display::update() {
    this->update_display();
    Widget::update();
}

void Text_display::set_text(Glyph_string text) {
    contents_ = std::move(text);
    this->update();
    top_line_ = 0;
    this->cursor.set_position({0, 0});
    text_changed(contents_);
}

void Text_display::insert(Glyph_string text, std::size_t index) {
    if (contents_.empty()) {
        this->append(std::move(text));
        return;
    }
    for (auto& glyph : text) {
        for (Attribute a : Attribute_list) {
            if (new_text_brush_.has_attribute(a)) {
                glyph.brush.add_attributes(a);
            }
        }
    }
    contents_.insert(std::begin(contents_) + index, std::begin(text),
                     std::end(text));
    this->update();
    text_changed(contents_);
}

void Text_display::append(Glyph_string text) {
    for (auto& glyph : text) {
        for (Attribute a : Attribute_list) {
            if (new_text_brush_.has_attribute(a)) {
                glyph.brush.add_attributes(a);
            }
        }
    }
    contents_.append(text);
    this->update();
    text_changed(contents_);
}

void Text_display::erase(std::size_t index, std::size_t length) {
    if (contents_.empty() || index >= contents_.size()) {
        return;
    }
    auto end = std::begin(contents_) + length + index;
    if (length == Glyph_string::npos) {
        end = std::end(contents_);
    }
    contents_.erase(std::begin(contents_) + index, end);
    this->update();
    text_changed(contents_);
}

void Text_display::pop_back() {
    if (contents_.empty()) {
        return;
    }
    contents_.pop_back();
    this->update();
    text_changed(contents_);
}

void Text_display::clear() {
    contents_.clear();
    this->cursor.set_x(0);
    this->cursor.set_y(0);
    this->update();
    text_changed(contents_);
}

void Text_display::scroll_up(std::size_t n) {
    if (n > this->top_line()) {
        top_line_ = 0;
    } else {
        top_line_ -= n;
    }
    this->update();
    scrolled_up(n);
    scrolled();
}

void Text_display::scroll_down(std::size_t n) {
    if (this->top_line() + n > this->last_line()) {
        top_line_ = this->last_line();
    } else {
        top_line_ += n;
    }
    this->update();
    scrolled_down(n);
    scrolled();
}

std::size_t Text_display::row_length(std::size_t y) const {
    const auto line = this->top_line() + y;
    return this->line_length(line);
}

std::size_t Text_display::index_at(std::size_t x, std::size_t y) const {
    auto line = this->top_line() + y;
    if (line >= display_state_.size()) {
        return this->contents_size();
    }
    auto info = display_state_.at(line);
    if (x >= info.length) {
        if (info.length == 0) {
            x = 0;
        } else if (this->top_line() + y != this->last_line()) {
            return this->first_index_at(this->top_line() + y + 1) - 1;
        } else if (this->top_line() + y == this->last_line()) {
            return this->contents_size();
        } else {
            x = info.length - 1;
        }
    }
    return info.start_index + x;
}

Point Text_display::display_position(std::size_t index) const {
    Point position;
    auto line = this->line_at(index);
    if (line < this->top_line()) {
        return position;
    }
    const auto last_shown_line = this->bottom_line();
    if (line > last_shown_line) {
        line = last_shown_line;
        index = this->last_index_at(line);
    } else if (index > this->contents_size()) {
        index = this->contents_size();
    }
    position.y = line - this->top_line();
    position.x = index - this->first_index_at(line);
    return position;
}

bool Text_display::paint_event() {
    Painter p{*this};
    std::size_t line_n{0};
    auto paint = [&p, &line_n, this](const Line_info& line) {
        auto sub_begin = std::begin(this->contents_) + line.start_index;
        auto sub_end = sub_begin + line.length;
        std::size_t start{0};
        switch (alignment_) {
            case Alignment::Left:
                start = 0;
                break;
            case Alignment::Center:
                start = (this->width() - line.length) / 2;
                break;
            case Alignment::Right:
                start = this->width() - line.length;
                break;
        }
        p.put(Glyph_string(sub_begin, sub_end), start, line_n++);
    };
    auto begin = std::begin(display_state_) + this->top_line();
    auto end = std::end(display_state_);
    if (display_state_.size() > this->top_line() + this->height()) {
        end = begin + this->height();  // maybe make this the initial value?
    }
    if (this->top_line() < display_state_.size()) {
        std::for_each(begin, end, paint);
    }
    return Widget::paint_event();
}

// TODO: Implement tab character. and newline?
// if (glyph.symbol == L'\n') {
//     move_cursor(*widget_, 0, widget_->cursor_y() + 1);
//     return;
// } else if (glyph.symbol == L'\t') {
//     // TODO move cursor to next x coord divisible by tablength
//     // textbox should account for it.
//     return;
// }
void Text_display::update_display(std::size_t from_line) {
    std::size_t begin = display_state_.at(from_line).start_index;
    display_state_.clear();
    if (this->width() == 0) {
        display_state_.push_back(Line_info{0, 0});
        return;
    }
    std::size_t start_index{0};
    std::size_t length{0};
    std::size_t last_space{0};
    for (std::size_t i{begin}; i < contents_.size(); ++i) {
        ++length;
        if (word_wrap_ && contents_.at(i).symbol == L' ') {
            last_space = length;
        }
        if (contents_.at(i).symbol == L'\n') {
            display_state_.push_back(Line_info{start_index, length - 1});
            start_index += length;
            length = 0;
        } else if (length == this->width()) {
            if (this->does_word_wrap() && last_space > 0) {
                i -= length - last_space;
                length = last_space;
                last_space = 0;
            }
            display_state_.push_back(Line_info{start_index, length});
            start_index += length;
            length = 0;
        }
    }
    display_state_.push_back(Line_info{start_index, length});
    // Reset top_line_ if out of bounds of new display.
    if (this->top_line() >= display_state_.size()) {
        top_line_ = this->last_line();
    }
}

std::size_t Text_display::line_at(std::size_t index) const {
    std::size_t line{0};
    for (const auto& info : display_state_) {
        if (info.start_index <= index) {
            ++line;
        } else {
            return line - 1;
        }
    }
    return this->last_line();
}

std::size_t Text_display::display_height() const {
    auto difference = 1 + this->last_line() - this->top_line();
    if (difference > this->height()) {
        difference = this->height();
    }
    return difference;
}

std::size_t Text_display::first_index_at(std::size_t line) const {
    if (line >= display_state_.size()) {
        line = display_state_.size() - 1;
    }
    return display_state_.at(line).start_index;
}

std::size_t Text_display::last_index_at(std::size_t line) const {
    const auto next_line = line + 1;
    if (next_line >= display_state_.size()) {
        return this->end_index();
    }
    return display_state_.at(next_line).start_index;
}

std::size_t Text_display::line_length(std::size_t line) const {
    if (line >= display_state_.size()) {
        line = display_state_.size() - 1;
    }
    return display_state_.at(line).length;
}

}  // namespace cppurses
