#include <termox/widget/widgets/text_view.hpp>

#include <algorithm>
#include <iterator>
#include <memory>
#include <string>
#include <utility>

#include <termox/painter/brush.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/painter/painter.hpp>
#include <termox/widget/align.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/wrap.hpp>

namespace ox {

Text_view::Text_view(Glyph_string text,
                     Align alignment,
                     Wrap wrap,
                     Brush insert_brush_)
    : insert_brush{std::move(insert_brush_)},
      contents_{std::move(text)},
      alignment_{alignment},
      wrap_{wrap}
{}

Text_view::Text_view(Parameters p)
    : Text_view{std::move(p.text), p.alignment, p.wrap,
                std::move(p.insert_brush)}
{}

void Text_view::set_text(Glyph_string text)
{
    contents_ = std::move(text);
    this->update();
    top_line_ = 0;
    this->cursor.set_position({0, 0});
    contents_modified(contents_);
}

auto Text_view::text() -> Glyph_string& { return contents_; }

auto Text_view::text() const -> Glyph_string const& { return contents_; }

void Text_view::set_alignment(Align type)
{
    alignment_ = type;
    this->update();
}

auto Text_view::alignment() const -> Align { return alignment_; }

void Text_view::set_wrap(Wrap w)
{
    wrap_ = w;
    this->update();
}

auto Text_view::wrap() const -> Wrap { return wrap_; }

void Text_view::insert(Glyph_string text, int index)
{
    if (index > contents_.size())
        return;
    if (contents_.empty()) {
        this->append(std::move(text));
        return;
    }
    for (auto& glyph : text)
        glyph = glyph | this->insert_brush.traits;
    contents_.insert(std::begin(contents_) + index, std::begin(text),
                     std::end(text));
    this->update();
    contents_modified(contents_);
}

void Text_view::append(Glyph_string text)
{
    for (auto& glyph : text)
        glyph = glyph | this->insert_brush.traits;
    contents_.append(text);
    this->update();
    contents_modified(contents_);
}

void Text_view::erase(int index, int length)
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

void Text_view::pop_back()
{
    if (contents_.empty())
        return;
    contents_.pop_back();
    this->update();
    contents_modified(contents_);
}

void Text_view::clear()
{
    contents_.clear();
    this->cursor.set_position({0, this->cursor.position().y});
    this->cursor.set_position({this->cursor.position().x, 0});
    this->update();
    contents_modified(contents_);
}

void Text_view::scroll_up(int n)
{
    if (n == 0)
        return;
    if (n > this->top_line())
        top_line_ = 0;
    else
        top_line_ -= n;
    this->update();
    scrolled_up(n);
    scrolled_to(top_line_);
}

void Text_view::scroll_down(int n)
{
    if (n == 0)
        return;
    if (this->top_line() + n > this->last_line())
        top_line_ = this->last_line();
    else
        top_line_ += n;
    this->update();
    scrolled_down(n);
    scrolled_to(top_line_);
}

auto Text_view::row_length(int y) const -> int
{
    const auto line = this->top_line() + y;
    return this->line_length(line);
}

auto Text_view::display_height() const -> int
{
    return std::min(1 + this->last_line() - this->top_line(),
                    this->area().height);
}

auto Text_view::line_count() const -> int { return display_state_.size(); }

void Text_view::set_top_line(int n)
{
    if (n < (int)display_state_.size())
        top_line_ = n;
    this->update();
}

auto Text_view::index_at(Point position) const -> int
{
    auto line = this->top_line() + position.y;
    if (line >= (int)display_state_.size())
        return this->text().size();
    auto const info = display_state_.at(line);
    if (position.x >= info.length) {
        if (info.length == 0)
            position.x = 0;
        else if (this->top_line() + position.y != this->last_line())
            return this->first_index_at(this->top_line() + position.y + 1) - 1;
        else
            return this->text().size();
    }
    return info.start_index + position.x;
}

auto Text_view::display_position(int index) const -> Point
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
    else if (index > this->text().size())
        index = this->text().size();
    position.y = line - this->top_line();
    position.x = index - this->first_index_at(line);
    return position;
}

void Text_view::update()
{
    // TODO
    // This call to update_display is required here, and not in paint_event.
    // Could probably be refactored so this can be in paint_event, more
    // efficient...
    this->update_display();
    Widget::update();
}

auto Text_view::paint_event(Painter& p) -> bool
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
                start = (this->area().width - line.length) / 2;
                break;
            case Align::Bottom:
            case Align::Right: start = this->area().width - line.length; break;
        }
        p.put(Glyph_string(sub_begin, sub_end), {start, line_n++});
    };
    auto const begin = std::next(std::cbegin(display_state_), this->top_line());
    auto const end   = [&] {
        auto const end_index = this->top_line() + this->area().height;
        if ((int)display_state_.size() > end_index)
            return std::next(begin, this->area().height);
        else
            return std::cend(display_state_);
    }();
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

auto Text_view::line_at(int index) const -> int
{
    auto line = 0;
    for (auto const& info : display_state_) {
        if (info.start_index <= index)
            ++line;
        else
            return line - 1;
    }
    return this->last_line();
}

auto Text_view::top_line() const -> int { return top_line_; }

auto Text_view::bottom_line() const -> int
{
    auto const line = this->top_line() + this->display_height() - 1;
    return line < 0 ? 0 : line;
}

auto Text_view::last_line() const -> int { return display_state_.size() - 1; }

auto Text_view::first_index_at(int line) const -> int
{
    if (line >= (int)display_state_.size())
        line = display_state_.size() - 1;
    return display_state_.at(line).start_index;
}

auto Text_view::last_index_at(int line) const -> int
{
    const auto next_line = line + 1;
    if (next_line >= (int)display_state_.size())
        return this->end_index();
    return display_state_.at(next_line).start_index;
}

auto Text_view::line_length(int line) const -> int
{
    if (line >= (int)display_state_.size())
        line = display_state_.size() - 1;
    return display_state_.at(line).length;
}

auto Text_view::end_index() const -> int { return this->text().size(); }

void Text_view::update_display(int from_line)
{
    auto const begin = display_state_.at(from_line).start_index;
    display_state_.clear();
    if (this->area().width == 0) {
        display_state_.push_back(Line_info{0, 0});
        return;
    }
    auto start_index = 0;
    auto length      = 0;
    auto last_space  = 0;
    for (auto i = begin; i < contents_.size(); ++i) {
        ++length;
        if ((this->wrap() == Wrap::Word) && (contents_.at(i).symbol == U' '))
            last_space = length;
        if (contents_.at(i).symbol == U'\n') {
            display_state_.push_back(Line_info{start_index, length - 1});
            start_index += length;
            length = 0;
        }
        else if (length == this->area().width) {
            if ((this->wrap() == Wrap::Word) && last_space > 0) {
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

auto text_view(Glyph_string text, Align alignment, Wrap wrap)
    -> std::unique_ptr<Text_view>
{
    return std::make_unique<Text_view>(std::move(text), alignment, wrap);
}

auto text_view(Text_view::Parameters p) -> std::unique_ptr<Text_view>
{
    return std::make_unique<Text_view>(std::move(p));
}

}  // namespace ox
