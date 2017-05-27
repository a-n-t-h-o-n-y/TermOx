#include "widget_module/widgets/text_display.hpp"

#include "painter_module/glyph_string.hpp"
#include "painter_module/painter.hpp"
#include "system_module/events/resize_event.hpp"
#include "widget_module/coordinate.hpp"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <utility>

namespace twf {

Text_display::Text_display(Glyph_string content)
    : contents_{std::move(content)} {
    this->update_display();
    this->Text_display::initialize();
}

void Text_display::initialize() {
    set_text = [this](const Glyph_string& s) { this->set_text_(s); };
    set_text.track(this->destroyed);
    insert = [this](const Glyph_string& s, std::size_t index) {
        this->insert_(s, index);
    };
    insert.track(this->destroyed);
    append = [this](const Glyph_string& s) { this->append_(s); };
    append.track(this->destroyed);
    erase = [this](std::size_t index) { this->erase_(index); };
    erase.track(this->destroyed);
    pop_back = [this] { this->pop_back_(); };
    pop_back.track(this->destroyed);
    clear = [this] { this->clear_(); };
    clear.track(this->destroyed);

    scroll_up = [this] { this->scroll_up_(1); };
    scroll_up.track(this->destroyed);
    scroll_down = [this] { this->scroll_down_(1); };
    scroll_down.track(this->destroyed);
    scroll_up_n = [this](std::size_t n) { this->scroll_up_(n); };
    scroll_up_n.track(this->destroyed);
    scroll_down_n = [this](std::size_t n) { this->scroll_down_(n); };
    scroll_down_n.track(this->destroyed);

    enable_word_wrap = [this] { this->enable_word_wrap_(); };
    enable_word_wrap.track(this->destroyed);
    disable_word_wrap = [this] { this->disable_word_wrap_(); };
    disable_word_wrap.track(this->destroyed);
    toggle_word_wrap = [this] {
        this->enable_word_wrap_(!this->does_word_wrap());
    };
    set_word_wrap = [this](bool enable) { this->enable_word_wrap_(enable); };
    set_word_wrap.track(this->destroyed);
}

void Text_display::set_text_(const Glyph_string& string) {
    contents_ = string;
    this->update_display();
    text_changed(contents_);
}

void Text_display::insert_(const Glyph_string& string, std::size_t index) {
    if (contents_.empty()) {
        this->append_(string);
        return;
    }
    contents_.insert(std::begin(contents_) + index, std::begin(string),
                     std::end(string));
    this->update_display();
    text_changed(contents_);
}

void Text_display::append_(const Glyph_string& string) {
    contents_.append(string);
    this->update_display();
    text_changed(contents_);
}

void Text_display::erase_(std::size_t index, std::size_t length) {
    if (contents_.empty() || index >= contents_.size()) {
        return;
    }
    auto end = std::begin(contents_) + length + index;
    if (length == Glyph_string::npos) {
        end = std::end(contents_);
    }
    contents_.erase(std::begin(contents_) + index, end);
    this->update_display();
    text_changed(contents_);
}

void Text_display::pop_back_() {
    if (contents_.empty()) {
        return;
    }
    contents_.pop_back();
    this->update_display();
    text_changed(contents_);
}

void Text_display::clear_() {
    contents_.clear();
    this->update_display();
    text_changed(contents_);
}

void Text_display::scroll_up_(std::size_t n) {
    if (n > this->top_line()) {
        top_line_ = 0;
    } else {
        top_line_ -= n;
    }
    this->update();
    scrolled_up(n);
    scrolled();
}

void Text_display::scroll_down_(std::size_t n) {
    if (this->top_line() + n > this->last_line()) {
        top_line_ = this->last_line();
    } else {
        top_line_ += n;
    }
    this->update();
    scrolled_down(n);
    scrolled();
}

void Text_display::enable_word_wrap_(bool enable) {
    word_wrap_ = enable;
    this->update_display();
}

void Text_display::disable_word_wrap_(bool disable) {
    word_wrap_ = !disable;
    this->update_display();
}

std::size_t Text_display::row_length(std::size_t y) const {
    auto line = this->top_line() + y;
    return this->line_length(line);
}

std::size_t Text_display::index_at(Coordinate position) const {
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

Coordinate Text_display::display_position(std::size_t index) const {
    Coordinate position;
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

bool Text_display::paint_event(const Paint_event& event) {
    Painter p{this};
    std::size_t line_n{0};
    auto paint = [&p, &line_n, this](const auto& line) {
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
    return Widget::paint_event(event);
}

bool Text_display::resize_event(const Resize_event& event) {
    Widget::resize_event(event);
    this->update_display();
    return true;
}

// TODO: Implement tab character.
void Text_display::update_display(std::size_t from_line) {
    auto begin = display_state_.at(from_line).start_index;
    display_state_.clear();
    if (this->width() == 0) {
        display_state_.push_back(line_info{0, 0});
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
            display_state_.push_back(line_info{start_index, length - 1});
            start_index += length;
            length = 0;
        } else if (length == this->width()) {
            if (this->does_word_wrap() && last_space > 0) {
                i -= length - last_space;
                length = last_space;
                last_space = 0;
            }
            display_state_.push_back(line_info{start_index, length});
            start_index += length;
            length = 0;
        }
    }
    display_state_.push_back(line_info{start_index, length});
    // Reset top_line_ if out of bounds of new display.
    if (this->top_line() >= display_state_.size()) {
        top_line_ = this->last_line();
    }
    this->update();
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

}  // namespace twf
