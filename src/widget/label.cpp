#include "widget/widgets/label.hpp"
#include "painter/painter.hpp"
#include "widget/focus_policy.hpp"
#include "widget/size_policy.hpp"

#include <cstddef>
#include <utility>

namespace cppurses {

Label::Label(Glyph_string text) : text_{std::move(text)} {
    this->focus_policy = Focus_policy::None;
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(1);
}

void Label::set_text(Glyph_string text) {
    text_ = std::move(text);
    this->update();
}

void Label::set_alignment(Alignment type) {
    alignment_ = type;
    this->update();
}

bool Label::paint_event() {
    Painter p{this};
    std::size_t start{0};
    Glyph_string visible_text{text_};
    switch (alignment_) {
        case Alignment::Left:
            start = 0;
            if (text_.length() > this->width()) {
                auto begin = std::begin(text_);
                visible_text = Glyph_string{begin, begin + this->width()};
            }
            break;
        case Alignment::Center:
            if (text_.length() < this->width()) {
                start = (this->width() - text_.length()) / 2;
            } else {
                start = 0;
            }
            if (text_.length() > this->width()) {
                auto begin = std::begin(text_);
                std::size_t diff = (text_.length() - this->width()) / 2;
                visible_text =
                    Glyph_string{begin + diff, begin + this->width() + diff};
            }
            break;
        case Alignment::Right:
            if (text_.length() < this->width()) {
                start = this->width() - text_.length();
            } else {
                start = 0;
            }
            if (text_.length() > this->width()) {
                std::size_t diff = text_.length() - this->width();
                visible_text =
                    Glyph_string{std::begin(text_) + diff, std::end(text_)};
            }
            break;
    }
    p.put(visible_text, start, 0);
    return Widget::paint_event();
}

}  // namespace cppurses
