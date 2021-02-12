#include <termox/widget/widgets/text_display.hpp>

#include <algorithm>
#include <iterator>
#include <string>
#include <utility>

#include <termox/painter/glyph_string.hpp>
#include <termox/painter/painter.hpp>
#include <termox/painter/trait.hpp>
#include <termox/widget/point.hpp>

namespace ox {

void Text_display::insert(Glyph_string text, int index)
{
    if (index > contents_.size())
        return;
    if (contents_.empty()) {
        this->append(std::move(text));
        return;
    }
    for (auto& glyph : text)
        glyph.brush.traits |= this->insert_brush.traits;
    contents_.insert(std::begin(contents_) + index, std::begin(text),
                     std::end(text));
    this->update();
    contents_modified(contents_);
}

void Text_display::append(Glyph_string text)
{
    for (auto& glyph : text)
        glyph.brush.traits |= this->insert_brush.traits;
    contents_.append(text);
    this->update();
    contents_modified(contents_);
}

void Text_display::erase(int index, int length)
{
    if (contents_.empty() || index >= contents_.size())
        return;
    auto const begin = std::begin(contents_);
    auto end         = begin + length + index;
    if (length == Glyph_string::npos)
        end = std::end(contents_);
    contents_.erase(std::begin(contents_) + index, end);
    this->update();
    contents_modified(contents_);
}

void Text_display::pop_back()
{
    if (contents_.empty())
        return;
    contents_.pop_back();
    this->update();
    contents_modified(contents_);
}

void Text_display::clear()
{
    contents_.clear();
    this->cursor.set_x(0);
    this->cursor.set_y(0);
    this->update();
    contents_modified(contents_);
}

void Text_display::scroll_up(int n)
{
    if (n > this->top_line())
        top_line_ = 0;
    else
        top_line_ -= n;
    this->update();
    scrolled_up(n);
    scrolled_to(top_line_);
}

void Text_display::scroll_down(int n)
{
    if (this->top_line() + n > this->last_line())
        top_line_ = this->last_line();
    else
        top_line_ += n;
    this->update();
    scrolled_down(n);
    scrolled_to(top_line_);
}

auto Text_display::index_at(Point position) const -> int
{
    auto line = this->top_line() + position.y;
    if (line >= (int)display_state_.size())
        return this->contents().size();
    auto const info = display_state_.at(line);
    if (position.x >= info.length) {
        if (info.length == 0)
            position.x = 0;
        else if (this->top_line() + position.y != this->last_line())
            return this->first_index_at(this->top_line() + position.y + 1) - 1;
        else if (this->top_line() + position.y == this->last_line())
            return this->contents().size();
        else  // TODO This can't be reached
            position.x = info.length - 1;
    }
    return info.start_index + position.x;
}

auto Text_display::display_position(int index) const -> Point
{
    auto position = Point{};
    auto line     = this->line_at(index);
    if (line < this->top_line())
        return position;
    auto const last_shown_line = this->bottom_line();
    if (line > last_shown_line) {
        line  = last_shown_line;
        index = this->last_index_at(line);
    }
    else if (index > this->contents().size())
        index = this->contents().size();
    position.y = line - this->top_line();
    position.x = index - this->first_index_at(line);
    return position;
}

auto Text_display::paint_event(Painter& p) -> bool
{
    auto line_n = 0;
    auto paint  = [&p, &line_n, this](Line_info const& line) {
        auto const sub_begin = std::begin(this->contents_) + line.start_index;
        auto const sub_end   = sub_begin + line.length;
        auto start           = 0;
        switch (alignment_) {
            case Align::Top:
            case Align::Left: start = 0; break;
            case Align::Center:
                start = (this->width() - line.length) / 2;
                break;
            case Align::Bottom:
            case Align::Right: start = this->width() - line.length; break;
        }
        p.put(Glyph_string(sub_begin, sub_end), {start, line_n++});
    };
    auto const begin = std::begin(display_state_) + this->top_line();
    auto end         = std::end(display_state_);
    if ((int)display_state_.size() > this->top_line() + this->height())
        end = begin + this->height();  // maybe make this the initial value?
    if (this->top_line() < (int)display_state_.size())
        std::for_each(begin, end, paint);
    return Widget::paint_event(p);
}

// TODO: Implement tab character. and newline?
// if (glyph.symbol == U'\n') {
//     move_cursor(*widget_, 0, widget_->cursor_y() + 1);
//     return;
// } else if (glyph.symbol == U'\t') {
//     // TODO move cursor to next x coord divisible by tablength
//     // textbox should account for it.
//     return;
// }

void Text_display::update_display(int from_line)
{
    auto const begin = display_state_.at(from_line).start_index;
    display_state_.clear();
    if (this->width() == 0) {
        display_state_.push_back(Line_info{0, 0});
        return;
    }
    auto start_index = 0;
    auto length      = 0;
    auto last_space  = 0;
    for (auto i = begin; i < contents_.size(); ++i) {
        ++length;
        if (this->word_wrap_enabled() and contents_.at(i).symbol == U' ')
            last_space = length;
        if (contents_.at(i).symbol == U'\n') {
            display_state_.push_back(Line_info{start_index, length - 1});
            start_index += length;
            length = 0;
        }
        else if (length == this->width()) {
            if (this->word_wrap_enabled() and last_space > 0) {
                i -= length - last_space;
                length     = last_space;
                last_space = 0;
            }
            display_state_.push_back(Line_info{start_index, length});
            start_index += length;
            length = 0;
        }
    }
    display_state_.push_back(Line_info{start_index, length});
    // Reset top_line_ if out of bounds of new display.
    if (this->top_line() >= (int)display_state_.size())
        top_line_ = this->last_line();
    line_count_changed(display_state_.size());
}

auto Text_display::line_at(int index) const -> int
{
    auto line = 0uL;
    for (auto const& info : display_state_) {
        if (info.start_index <= index)
            ++line;
        else
            return line - 1;
    }
    return this->last_line();
}

}  // namespace ox
