#include <termox/widget/widgets/detail/textline_core.hpp>

#include <algorithm>
#include <cassert>
#include <iterator>
#include <utility>

#include <termox/painter/glyph.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/widget/align.hpp>

namespace ox::detail {

Textline_core::Textline_core(ox::Glyph_string initial,
                             int display_length,
                             ox::Align alignment,
                             int cursor_index)
    : text_{std::move(initial)},
      display_length_{display_length},
      alignment_{alignment},
      cursor_index_{cursor_index}
{}

void Textline_core::set_text(ox::Glyph_string x)
{
    text_ = std::move(x);
    if (cursor_index_ > text_.size()) {
        cursor_index_ = 0;
        anchor_index_ = 0;
    }
}

auto Textline_core::text() const noexcept -> ox::Glyph_string const&
{
    return text_;
}

void Textline_core::clear()
{
    text_         = U"";
    cursor_index_ = 0;
    anchor_index_ = 0;
}

void Textline_core::set_display_length(int x)
{
    display_length_ = x;
    if (alignment_ == ox::Align::Right &&
        ((text_.size() - anchor_index_) < display_length_) &&
        anchor_index_ != 0) {
        anchor_index_ = std::max(text_.size() - (display_length_ - 1), 0);
    }
    auto const last_display_index =
        anchor_index_ + std::max(display_length_ - 1, 0);
    if (cursor_index_ > last_display_index)
        cursor_index_ = last_display_index;
}

auto Textline_core::display_length() const noexcept -> int
{
    return display_length_;
}

void Textline_core::set_alignment(ox::Align x)
{
    assert(x == ox::Align::Left || x == ox::Align::Right);
    alignment_ = x;
    this->set_display_length(display_length_);  // reset anchor_index_
}

auto Textline_core::alignment() const noexcept -> ox::Align
{
    return alignment_;
}

void Textline_core::increment_cursor()
{
    // Cursor index can be == text_.size()
    if (cursor_index_ < text_.size())
        ++cursor_index_;
    if (cursor_index_ - anchor_index_ >= display_length_)
        ++anchor_index_;
}

void Textline_core::decrement_cursor()
{
    if (cursor_index_ != 0)
        --cursor_index_;
    if (cursor_index_ < anchor_index_)
        --anchor_index_;
}

void Textline_core::move_cursor_to_display(int x)
{
    if (alignment_ == ox::Align::Right) {
        x -= std::max(display_length_ - 1 - (text_.size() - anchor_index_), 0);
        x = std::max(x, 0);
    }
    cursor_index_ = std::min(anchor_index_ + x, text_.size());
}

void Textline_core::set_cursor_to_index(int i)
{
    cursor_index_ = i;
    if (cursor_index_ < anchor_index_)
        anchor_index_ = cursor_index_;
    else if (display_length_ != 0 &&
             (cursor_index_ - anchor_index_) > display_length_) {
        anchor_index_ = cursor_index_ - display_length_;
    }
}

void Textline_core::cursor_home()
{
    cursor_index_ = 0;
    anchor_index_ = 0;
}

void Textline_core::cursor_end()
{
    cursor_index_ = text_.size();
    if (cursor_index_ - anchor_index_ > display_length_)
        anchor_index_ = std::max(text_.size() - display_length_ + 1, 0);
}

void Textline_core::insert_at_cursor(ox::Glyph x)
{
    auto const at = std::next(std::begin(text_), cursor_index_);
    text_.insert(at, x);
    this->increment_cursor();
}

void Textline_core::erase_at_cursor()
{
    if (cursor_index_ == text_.size())
        return;
    auto const at = std::next(std::begin(text_), cursor_index_);
    text_.erase(at);
    if (alignment_ == ox::Align::Right)
        anchor_index_ = std::max(anchor_index_ - 1, 0);
    if (cursor_index_ > text_.size())
        this->decrement_cursor();
}

void Textline_core::erase_before_cursor()
{
    if (cursor_index_ == 0)
        return;
    auto const at = std::next(std::begin(text_), cursor_index_ - 1);
    text_.erase(at);
    if (alignment_ == ox::Align::Right)
        anchor_index_ = std::max(anchor_index_ - 1, 0);
    this->decrement_cursor();
}

auto Textline_core::cursor_position() const noexcept -> int
{
    return cursor_index_ - anchor_index_;
}

auto Textline_core::display_substr() const -> ox::Glyph_string
{
    auto const first = std::next(std::begin(text_), anchor_index_);
    auto const last  = [&] {
        auto const end_index =
            std::min(anchor_index_ + display_length_, text_.size());
        return std::next(std::begin(text_), end_index);
    }();
    auto result = ox::Glyph_string{first, last};
    if (result.size() < display_length_)
        result.push_back(U' ');  // For Cursor
    return result;
}

}  // namespace ox::detail
