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
    auto end = (length == Glyph_string::npos) ? std::end(contents_)
                                              : std::begin(contents_) + length;
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
    return display_state_.at(this->top_line() + y).length;
}

std::size_t Text_display::string_index(Coordinate position) const {
    return this->string_index(position.x, position.y);
}

std::size_t Text_display::string_index(std::size_t x, std::size_t y) const {
    auto info = display_state_.at(y + this->top_line());
    if (info.length == 0) {
        return info.start_index;
    }
    if (x >= info.length) {
        x = info.length;
    }
    return info.start_index + x;
}

Coordinate Text_display::display_position(std::size_t index) const {
    Coordinate result;
    result.y = this->line_at(index) - this->top_line();
    result.x =
        index - display_state_.at(result.y + this->top_line()).start_index;
    return result;
}

bool Text_display::paint_event(const Paint_event& event) {
    Painter p{this};
    std::size_t line_n{0};
    auto paint = [&p, &line_n, this](const auto& line) {
        auto sub_begin = std::begin(contents_) + line.start_index;
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

void Text_display::update_display(std::size_t from_line) {
    // Implement '\t' character, word wrapping and 'from_line' optimization.
    display_state_.clear();
    std::size_t start_index{0};
    std::size_t length{0};
    for (std::size_t i{0}; i < contents_.size(); ++i) {
        ++length;
        if (contents_.at(i).str() == "\n") {
            display_state_.push_back(line_info{start_index, length - 1});
            start_index += length;
            length = 0;
        } else if (length == this->width()) {
            // new-line removal after non-zero line
            std::size_t nl_offset{0};
            if (i != (contents_.size() - 1)) {
                if (contents_.at(i + 1).str() == "\n") {
                    ++nl_offset;
                    ++i;
                }
            }
            display_state_.push_back(line_info{start_index, length});
            start_index += length + nl_offset;
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
        return contents_.empty() ? 0 : contents_.size() - 1;
    }
    auto pred = [index](auto& info) {
        return (info.start_index + info.length) > index;
    };
    auto pos = std::find_if(std::begin(display_state_),
                            std::end(display_state_), pred);
    return pos - std::begin(display_state_);
}

std::size_t Text_display::top_line() const {
    return top_line_;
}

std::size_t Text_display::last_line() const {
    return display_state_.empty() ? 0 : display_state_.size() - 1;
}

std::size_t Text_display::index_at(std::size_t line) const {
    return display_state_.at(line).start_index;
}

}  // namespace twf
