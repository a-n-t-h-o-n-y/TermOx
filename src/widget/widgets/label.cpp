#include <termox/widget/widgets/label.hpp>

#include <memory>
#include <utility>

#include <termox/painter/glyph_string.hpp>
#include <termox/painter/painter.hpp>
#include <termox/widget/align.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/growth.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>

#ifdef __clang__
#    include <termox/widget/layouts/opposite.hpp>
#endif

namespace {

[[nodiscard]] auto constexpr left_top_offset() -> int { return 0; }

[[nodiscard]] auto constexpr center_offset(int text_length, int box_length)
    -> int
{
    return text_length > box_length ? 0 : (box_length - text_length) / 2;
}

[[nodiscard]] auto constexpr right_bottom_offset(int text_length,
                                                 int box_length) -> int
{
    return text_length > box_length ? 0 : box_length - text_length;
}

/// Offsets a given \p position by the extra length parameters
[[nodiscard]] auto constexpr apply_origin_offset(int position, int left) -> int
{
    position -= position < left ? position : left;
    return position;
}

}  // namespace

namespace ox {

template <template <typename> typename Layout_t>
Label<Layout_t>::Label(Glyph_string text,
                       Align alignment,
                       int extra_left,
                       int extra_right,
                       Growth growth_strategy)
    : text_{std::move(text)},
      alignment_{alignment},
      extra_left_{extra_left},
      extra_right_{extra_right},
      growth_strategy_{growth_strategy}
{
    if constexpr (is_vertical)
        *this | pipe::fixed_width(1);
    else
        *this | pipe::fixed_height(1);

    if (growth_strategy_ == Growth::Dynamic) {
        if constexpr (is_vertical)
            *this | pipe::fixed_height(text_.size());
        else
            *this | pipe::fixed_width(text_.size());
    }
}

template <template <typename> typename Layout_t>
Label<Layout_t>::Label(Parameters p)
    : Label(std::move(p.text),
            p.alignment,
            p.extra_left,
            p.extra_right,
            p.growth_strategy)
{}

template <template <typename> typename Layout_t>
void Label<Layout_t>::set_text(Glyph_string text)
{
    if (growth_strategy_ == Growth::Dynamic) {
        if constexpr (is_vertical) {
            if (text.size() != this->area().height)
                *this | pipe::fixed_height(text.size());
        }
        else {
            if (text.size() != this->area().width)
                *this | pipe::fixed_width(text.size());
        }
    }
    text_ = std::move(text);
    this->update_offset();
}

template <template <typename> typename Layout_t>
auto Label<Layout_t>::text() const noexcept -> Glyph_string const&
{
    return text_;
}

template <template <typename> typename Layout_t>
void Label<Layout_t>::set_alignment(Align x)
{
    alignment_ = x;
    this->update_offset();
}

template <template <typename> typename Layout_t>
auto Label<Layout_t>::alignment() const noexcept -> Align
{
    return alignment_;
}

template <template <typename> typename Layout_t>
void Label<Layout_t>::set_extra_left(int x)
{
    extra_left_ = x;
    this->update_offset();
}

template <template <typename> typename Layout_t>
auto Label<Layout_t>::extra_left() const noexcept -> int
{
    return extra_left_;
}

template <template <typename> typename Layout_t>
void Label<Layout_t>::set_extra_right(int x)
{
    extra_right_ = x;
    this->update_offset();
}

template <template <typename> typename Layout_t>
auto Label<Layout_t>::extra_right() const noexcept -> int
{
    return extra_right_;
}

template <template <typename> typename Layout_t>
void Label<Layout_t>::set_growth_strategy(Growth type)
{
    growth_strategy_ = type;
    if (growth_strategy_ == Growth::Dynamic) {
        if constexpr (is_vertical)
            *this | pipe::fixed_height(text_.size());
        else
            *this | pipe::fixed_width(text_.size());
    }
}

template <template <typename> typename Layout_t>
auto Label<Layout_t>::growth_strategy() const noexcept -> Growth
{
    return growth_strategy_;
}

template <template <typename> typename Layout_t>
auto Label<Layout_t>::paint_event(Painter& p) -> bool
{
    if constexpr (is_vertical)
        this->paint_vertical(p);
    else
        this->paint_horizontal(p);
    return Widget::paint_event(p);
}

template <template <typename> typename Layout_t>
auto Label<Layout_t>::resize_event(Area new_size, Area old_size) -> bool
{
    this->update_offset();
    return Widget::resize_event(new_size, old_size);
}

template <template <typename> typename Layout_t>
void Label<Layout_t>::update_offset()
{
    auto const box_length = [this] {
        if constexpr (is_vertical)
            return this->area().height;
        else
            return this->area().width;
    }();

    offset_ =
        this->find_offset(text_.size(), box_length, extra_left_, extra_right_);
    this->update();
}

template <template <typename> typename Layout_t>
void Label<Layout_t>::paint_vertical(Painter& p)
{
    for (auto i = 0; i < text_.size() && i < this->area().height; ++i)
        p.put(text_[i], {0, offset_ + i});
}

template <template <typename> typename Layout_t>
void Label<Layout_t>::paint_horizontal(Painter& p)
{
    p.put(text_, {offset_, 0});
}

template <template <typename> typename Layout_t>
auto Label<Layout_t>::find_offset(int text_length,
                                  int box_length,
                                  int extra_left,
                                  int extra_right) -> int
{
    switch (alignment_) {
        case Align::Left:
        case Align::Top: return ::left_top_offset();

        case Align::Center:
            return ::apply_origin_offset(
                ::center_offset(text_length,
                                box_length + extra_left + extra_right),
                extra_left);

        case Align::Right:
        case Align::Bottom:
            return ::right_bottom_offset(text_length, box_length);
    }
    return 0;
}

template class Label<layout::Horizontal>;
template class Label<layout::Vertical>;

#ifdef __clang__  // Clang 12.0 needs these to be explicit, GCC 11.1 does not.
template class Label<layout::Opposite_template<layout::Horizontal<>>::type>;
template class Label<layout::Opposite_template<layout::Vertical<>>::type>;
#endif

template <template <typename> typename Layout_t>
auto label(Glyph_string text,
           Align alignment,
           int extra_left,
           int extra_right,
           Growth growth_strategy) -> std::unique_ptr<Label<Layout_t>>
{
    return std::make_unique<Label<Layout_t>>(
        std::move(text), alignment, extra_left, extra_right, growth_strategy);
}

template auto label(Glyph_string, Align, int, int, Growth)
    -> std::unique_ptr<HLabel>;

template auto label(Glyph_string, Align, int, int, Growth)
    -> std::unique_ptr<VLabel>;

template <template <typename> typename Layout_t>
auto label(typename Label<Layout_t>::Parameters p)
    -> std::unique_ptr<Label<Layout_t>>
{
    return std::make_unique<Label<Layout_t>>(std::move(p));
}

template auto label(HLabel::Parameters) -> std::unique_ptr<HLabel>;
template auto label(VLabel::Parameters) -> std::unique_ptr<VLabel>;

auto hlabel(Glyph_string text,
            Align alignment,
            int extra_left,
            int extra_right,
            Growth growth_strategy) -> std::unique_ptr<HLabel>
{
    return std::make_unique<HLabel>(std::move(text), alignment, extra_left,
                                    extra_right, growth_strategy);
}

auto hlabel(HLabel::Parameters p) -> std::unique_ptr<HLabel>
{
    return std::make_unique<HLabel>(std::move(p));
}

auto vlabel(Glyph_string text,
            Align alignment,
            int extra_left,
            int extra_right,
            Growth growth_strategy) -> std::unique_ptr<VLabel>
{
    return std::make_unique<VLabel>(std::move(text), alignment, extra_left,
                                    extra_right, growth_strategy);
}

auto vlabel(VLabel::Parameters p) -> std::unique_ptr<VLabel>
{
    return std::make_unique<VLabel>(std::move(p));
}

}  // namespace ox
