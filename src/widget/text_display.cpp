#include "widget/widgets/text_display.hpp"
#include "painter/glyph_string.hpp"
#include "painter/painter.hpp"
#include "widget/coordinates.hpp"

#include <signals/signal.hpp>
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string>
#include <utility>

namespace cppurses {

void Text_display::update() {
    this->update_display();
    Widget::update();
}

Text_display::Text_display(Glyph_string content)
    : contents_{std::move(content)} {}

void Text_display::set_text(Glyph_string text) {
    contents_ = std::move(text);
    this->update();
    text_changed(contents_);
}

void Text_display::insert(Glyph_string text, std::size_t index) {
    if (contents_.empty()) {
        this->append(std::move(text));
        return;
    }
    for (auto& glyph : text) {
        for (auto& attr : new_text_brush_.attributes()) {
            glyph.brush().add_attributes(attr);
        }
    }
    contents_.insert(std::begin(contents_) + index, std::begin(text),
                     std::end(text));
    this->update();
    text_changed(contents_);
}

void Text_display::append(Glyph_string text) {
    for (auto& glyph : text) {
        for (auto& attr : new_text_brush_.attributes()) {
            glyph.brush().add_attributes(attr);
        }
    }
    contents_.append(std::move(text));
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
    this->move_cursor_x(0);
    this->move_cursor_y(0);
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

void Text_display::enable_word_wrap(bool enable) {
    word_wrap_ = enable;
    this->update();
}

void Text_display::disable_word_wrap(bool disable) {
    word_wrap_ = !disable;
    this->update();
}

void Text_display::toggle_word_wrap() {
    word_wrap_ = !word_wrap_;
    this->update();
}

void Text_display::add_new_text_attribute(Attribute attr) {
    new_text_brush_.add_attributes(attr);
}

void Text_display::remove_new_text_attribute(Attribute attr) {
    new_text_brush_.remove_attribute(attr);
}

void Text_display::clear_new_text_attributes() {
    new_text_brush_.clear_attributes();
}

std::size_t Text_display::row_length(std::size_t y) const {
    auto line = this->top_line() + y;
    return this->line_length(line);
}

std::size_t Text_display::index_at(Coordinates position) const {
    return this->index_at(position.x, position.y);
}

std::size_t Text_display::index_at(std::size_t x, std::size_t y) const {
    auto line = this->top_line() + y;
    if (line >= display_state_.size()) {
        return contents_size();
    }
    auto info = display_state_.at(line);
    if (x >= info.length) {
        if (info.length == 0) {
            x = 0;
        } else if (this->top_line() + y != this->last_line()) {
            return this->first_index_at(this->top_line() + y + 1) - 1;
        } else if (this->top_line() + y == this->last_line()) {
            return contents_size();
        } else {
            x = info.length - 1;
        }
    }
    return info.start_index + x;
}

Coordinates Text_display::display_position(std::size_t index) const {
    Coordinates position;
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
    Painter p{this};
    std::size_t line_n{0};
    auto paint = [&p, &line_n, this](const Line_info& line) {
        auto sub_begin = std::begin(this->contents_) + line.start_index;
        auto sub_end = sub_begin + line.length;
        p.put(Glyph_string(sub_begin, sub_end), 0, line_n++);
    };
    auto begin = std::begin(display_state_) + this->top_line();
    auto end = std::end(display_state_);
    if (display_state_.size() > this->top_line() + this->height()) {
        end = begin + this->height();
    }
    if (this->top_line() < display_state_.size()) {
        std::for_each(begin, end, paint);
    }
    return Widget::paint_event();
}

// TODO: Implement tab character.
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
        if (word_wrap_ && contents_.at(i).str() == " ") {
            last_space = length;
        }
        if (contents_.at(i).str() == "\n") {
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

std::size_t Text_display::top_line() const {
    return top_line_;
}

std::size_t Text_display::bottom_line() const {
    return this->top_line() + this->display_height() - 1;
}

std::size_t Text_display::last_line() const {
    return display_state_.size() - 1;
}

std::size_t Text_display::n_of_lines() const {
    return display_state_.size();
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

std::size_t Text_display::end_index() const {
    if (this->contents_empty()) {
        return 0;
    }
    return this->contents_size() - 1;
}

}  // namespace cppurses
