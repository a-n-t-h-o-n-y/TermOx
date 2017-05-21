#include "widget_module/widgets/text_display.hpp"
#include "painter_module/painter.hpp"
#include "widget_module/coordinate.hpp"
#include "system_module/events/paint_event.hpp"
#include "system_module/events/resize_event.hpp"
#include <algorithm>
#include <cstddef>
#include <utility>

namespace twf {

Text_display::Text_display(Glyph_string content)
    : contents_{std::move(content)} {
    this->update_display();
}

void Text_display::insert(const Glyph_string& string, std::size_t index) {
    if (contents_.empty()) {
        contents_.append(string);
    }
    contents_.insert(std::begin(contents_) + index, std::begin(string),
                     std::end(string));
    this->update_display(this->line_at(index));
}

void Text_display::append(const Glyph_string& string) {
    contents_.append(string);
    this->update_display(this->last_line());
}

void Text_display::erase(std::size_t index, std::size_t length) {
    if (contents_.empty() || index >= contents_.size()) {
        return;
    }
    auto end = (length == Glyph_string::npos)
                   ? std::end(contents_)
                   : std::begin(contents_) + length + index;
    contents_.erase(std::begin(contents_) + index, end);
    this->update_display(this->line_at(index));
}

void Text_display::pop_back() {
    if (contents_.empty()) {
        return;
    }
    contents_.pop_back();
    this->update_display(last_line());
}

void Text_display::clear() {
    contents_.clear();
    this->update_display();
}

void Text_display::scroll_up(std::size_t n) {
    if (n > this->top_line()) {
        top_line_ = 0;
    } else {
        top_line_ -= n;
    }
}

void Text_display::scroll_down(std::size_t n) {
    if (this->top_line() + n > this->last_line()) {
        top_line_ = this->last_line();
    } else {
        top_line_ += n;
    }
}

std::size_t Text_display::row_length(std::size_t y) const {
    if (display_state_.empty()) {
        return 0;
    }
    auto line = this->top_line() + y;
    if (line >= display_state_.size()) {
        line = display_state_.size() - 1;
    }
    return display_state_.at(line).length;
}

std::size_t Text_display::string_index(Coordinate position) const {
    return this->string_index(position.x, position.y);
}

std::size_t Text_display::string_index(std::size_t x, std::size_t y) const {
    if (display_state_.empty()) {
        return 0;
    }
    auto line = this->top_line() + y;
    if (line >= display_state_.size()) {
        line = display_state_.size() - 1;
    }
    auto info = display_state_.at(line);
    if (info.length == 0) {
        return info.start_index;
    }
    if (x >= info.length) {
        x = info.length - 1;//
    }
    return info.start_index + x;
}

Coordinate Text_display::display_position(std::size_t index) const {
    Coordinate result;
    auto line = this->line_at(index);
    result.y = line - this->top_line();
    result.x = index - display_state_.at(line).start_index;
    return result;
}

bool Text_display::paint_event(const Paint_event& event) {
    Painter p{this};
    std::size_t line_n{0};
    Glyph_string contents_wo_nl{this->contents_};
    std::replace_if(std::begin(contents_wo_nl), std::end(contents_wo_nl),
                    [](auto& g) { return g.str() == "\n"; }, Glyph(" "));

    auto paint = [&p, &line_n, &contents_wo_nl, this](const auto& line) {
        auto sub_begin = std::begin(contents_wo_nl) + line.start_index;
        auto sub_end = sub_begin + line.length;
        p.put(Glyph_string(sub_begin, sub_end), 0, line_n++);
    };
    auto begin = std::begin(display_state_) + this->top_line();
    auto end = std::end(display_state_);
    if (display_state_.size() > this->top_line() + this->height()) {
        end = begin + this->height();
    }
    std::for_each(begin, end, paint);
    return Widget::paint_event(event);
}

bool Text_display::resize_event(const Resize_event& event) {
    Widget::resize_event(event);
    this->update_display();
    this->update();
    return true;
}

// TODO: Implement tab character and 'from_line' optimization.
void Text_display::update_display(std::size_t from_line) {
    display_state_.clear();
    std::size_t start_index{0};
    std::size_t length{0};
    std::size_t last_space{0};
    for (std::size_t i{0}; i < contents_.size(); ++i) {
        ++length;
        if (word_wrap_ && contents_.at(i).str() == " ") {
            last_space = length;
        }
        if (contents_.at(i).str() == "\n") {
            display_state_.push_back(line_info{start_index, length - 1});
            start_index += length;
            length = 0;
        } else if (length == this->width()) {
            std::size_t step_over{0};
            if (this->width() > 1) {
                if (i + 1 != contents_.size()) {
                    auto next_char = contents_.at(i + 1).str();
                    if (next_char == "\n" || next_char == " ") {
                        ++i;
                        last_space = 0;
                        ++step_over;
                    }
                }
                if (this->word_wrap() && last_space > 1) {
                    i -= length - last_space;
                    length = last_space - 1;
                    last_space = 0;
                    ++step_over;
                }
            }
            display_state_.push_back(line_info{start_index, length});
            start_index += length + step_over;
            length = 0;
        }
    }
    display_state_.push_back(line_info{start_index, length});
    if (top_line_ >= display_state_.size()) {
        if (display_state_.empty()) {
            top_line_ = 0;
        }
        top_line_ = display_state_.size() - 1;
    }
}

std::size_t Text_display::line_at(std::size_t index) const {
    if (index + 1 > contents_.size()) {
        return display_state_.empty() ? 0 : display_state_.size() - 1;
    }
    auto pred = [index](auto& info) {
        return (info.start_index + info.length) > index;
    };
    auto pos = std::find_if(std::begin(display_state_),
                            std::end(display_state_), pred);
    return pos - std::begin(display_state_);
}

std::size_t Text_display::number_of_rows() const {
    if (this->last_line() == 0) {
        return 0;
    }
    auto lower_rows = this->last_line() - this->top_line() + 1;
    if (lower_rows >= this->height()) {
        return this->height();
    }
    return lower_rows;
}

std::size_t Text_display::top_line() const {
    return top_line_;
}

std::size_t Text_display::last_line() const {
    return display_state_.empty() ? 0 : display_state_.size() - 1;
}

std::size_t Text_display::index_at(std::size_t line) const {
    if (display_state_.empty()) {
        return 0;
    }
    if (line >= display_state_.size()) {
        line = display_state_.size() - 1;
    }
    return display_state_.at(line).start_index;
}

std::size_t Text_display::line_length(std::size_t line) const {
    if (display_state_.empty()) {
        return 0;
    }
    if (line >= display_state_.size()) {
        line = display_state_.size() - 1;
    }
    return display_state_.at(line).length;
}

}  // namespace twf
