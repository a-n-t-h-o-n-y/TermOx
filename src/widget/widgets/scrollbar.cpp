#include <termox/widget/widgets/scrollbar.hpp>

#include <cmath>
#include <cstddef>
#include <memory>
#include <utility>

#include <termox/painter/color.hpp>
#include <termox/painter/painter.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>

namespace {

template <bool is_vertical>
[[nodiscard]] auto constexpr point(int position) -> ox::Point
{
    if constexpr (is_vertical)
        return {0, position};
    else
        return {position, 0};
}

[[nodiscard]] auto constexpr slider_position(int max_length,
                                             double line_count,
                                             double position,
                                             std::size_t slider_length) -> int
{
    if (line_count == 0. || line_count == 1.)
        return 0;
    double const ratio = position / (line_count - 1);
    auto const length  = max_length - slider_length;
    return std::round(ratio * length);
}

[[nodiscard]] auto constexpr slider_length(double max_length, double line_count)
    -> std::size_t
{
    if (max_length == 0. || line_count == 0.)
        return 0;
    return std::ceil((1. / line_count) * max_length);
}

}  // namespace

namespace ox {

template <template <typename> typename Layout_t>
Scrollbar<Layout_t>::Middle::Middle(std::size_t size, std::size_t position)
    : size_{size}, position_{position}
{
    this->set_bar_fg(Color::Foreground);
    this->set_bar_bg(Color::Dark_gray);
}

template <template <typename> typename Layout_t>
Scrollbar<Layout_t>::Middle::Middle(Parameters p) : Middle{p.size, p.position}
{}

template <template <typename> typename Layout_t>
void Scrollbar<Layout_t>::Middle::set_position(std::size_t p)
{
    position_ = p;
    this->update_length_and_position();
    this->update();
}

template <template <typename> typename Layout_t>
void Scrollbar<Layout_t>::Middle::set_size(std::size_t s)
{
    size_ = s;
    this->update_length_and_position();
    this->update();
}

template <template <typename> typename Layout_t>
void Scrollbar<Layout_t>::Middle::set_bar_fg(Color c)
{
    bar_ | fg(c);
    this->update();
}

template <template <typename> typename Layout_t>
void Scrollbar<Layout_t>::Middle::set_bar_bg(Color c)
{
    if constexpr (is_vertical)
        *this | bg(c);
    else
        *this | pipe::wallpaper(U'▬' | fg(c));
    this->update();
}

template <template <typename> typename Layout_t>
auto Scrollbar<Layout_t>::Middle::find_position_from_point(Point p)
    -> std::size_t
{
    auto const length = (int)this->max_length() - (int)slider_length_;
    if (length < 1 || size_ == 0)
        return 0;
    double const at = is_vertical ? p.y : p.x;
    double ratio    = at / length;
    if (ratio > 1.)
        ratio = 1.;
    return ratio * (size_ - 1);
}

template <template <typename> typename Layout_t>
auto Scrollbar<Layout_t>::Middle::paint_event(Painter& p) -> bool
{
    if (size_ == 0 || position_ == invalid_position || slider_length_ == 0)
        return Widget::paint_event(p);
    auto const begin  = point<is_vertical>(slider_position_);
    auto const length = point<is_vertical>(slider_length_ - 1);
    if constexpr (is_vertical)
        p.vline(bar_, begin, begin + length);
    else
        p.hline(bar_, begin, begin + length);
    return Widget::paint_event(p);
}

template <template <typename> typename Layout_t>
auto Scrollbar<Layout_t>::Middle::resize_event(Area new_size, Area old_size)
    -> bool
{
    this->update_length_and_position();
    return Widget::resize_event(new_size, old_size);
}

template <template <typename> typename Layout_t>
void Scrollbar<Layout_t>::Middle::update_length_and_position()
{
    // Order Matters, position depends on length
    auto const length = this->max_length();
    slider_length_    = ::slider_length(length, size_);
    slider_position_ =
        ::slider_position(length, size_, position_, slider_length_);
}

template <template <typename> typename Layout_t>
auto Scrollbar<Layout_t>::Middle::max_length() const -> std::size_t
{
    if constexpr (is_vertical)
        return this->height();
    else
        return this->width();
}

template <template <typename> typename Layout_t>
Scrollbar<Layout_t>::Scrollbar(std::size_t size, std::size_t position)
    : size_{size}, position_{position}
{
    using namespace pipe;
    if constexpr (is_vertical) {
        *this | fixed_width(1);
        decrement_btn | fixed_height(1);
        middle | expanding_height(0);
        increment_btn | fixed_height(1);
    }
    else {
        *this | fixed_height(1);
        decrement_btn | fixed_width(1);
        middle | expanding_width(0);
        increment_btn | fixed_width(1);
    }

    decrement_btn.pressed.connect([this] { this->decrement_position(); });
    increment_btn.pressed.connect([this] { this->increment_position(); });

    decrement_btn.install_event_filter(*this);
    increment_btn.install_event_filter(*this);
    middle.install_event_filter(*this);
}

template <template <typename> typename Layout_t>
Scrollbar<Layout_t>::Scrollbar(Parameters p) : Scrollbar{p.size, p.position}
{}

template <template <typename> typename Layout_t>
auto Scrollbar<Layout_t>::get_size() const -> std::size_t
{
    return size_;
}

template <template <typename> typename Layout_t>
void Scrollbar<Layout_t>::set_size(std::size_t s)
{
    size_ = s;
    if (position_ == invalid_position && size_ != 0)
        this->set_position(0);
    else if (position_ >= size_)
        this->set_position(size_ - 1);
    middle.set_size(s);
}

template <template <typename> typename Layout_t>
void Scrollbar<Layout_t>::increment_size()
{
    this->set_size(this->get_size() + 1);
}

template <template <typename> typename Layout_t>
void Scrollbar<Layout_t>::decrement_size()
{
    if (this->get_size() == 0)
        return;
    this->set_size(this->get_size() - 1);
}

template <template <typename> typename Layout_t>
auto Scrollbar<Layout_t>::get_position() const -> std::size_t
{
    return position_;
}

template <template <typename> typename Layout_t>
void Scrollbar<Layout_t>::set_position(std::size_t p)
{
    if (size_ == 0)
        return;
    position_ = p < size_ ? p : size_ - 1;
    new_position.emit(position_);
    middle.set_position(p);
}

template <template <typename> typename Layout_t>
void Scrollbar<Layout_t>::decrement_position()
{
    if (position_ == invalid_position)
        return;
    if (position_ == 0)
        return;
    this->set_position(position_ - 1);
    decremented();
}

template <template <typename> typename Layout_t>
void Scrollbar<Layout_t>::increment_position()
{
    if (position_ + 1 == size_)
        return;
    this->set_position(position_ + 1);
    incremented();
}

template <template <typename> typename Layout_t>
auto Scrollbar<Layout_t>::mouse_wheel_event_filter(Widget&, Mouse const& m)
    -> bool
{
    return this->handle_wheel(m.button);
}

template <template <typename> typename Layout_t>
auto Scrollbar<Layout_t>::mouse_press_event_filter(Widget& w, Mouse const& m)
    -> bool
{
    if (&w != &middle)
        return false;
    if (m.button == Mouse::Button::Left)
        this->set_position(middle.find_position_from_point(m.at));
    return true;
}

template <template <typename> typename Layout_t>
auto Scrollbar<Layout_t>::mouse_move_event_filter(Widget& w, Mouse const& m)
    -> bool
{
    if (&w != &middle)
        return false;
    if (m.button == Mouse::Button::Left)
        this->set_position(middle.find_position_from_point(m.at));
    return true;
}

template <template <typename> typename Layout_t>
auto Scrollbar<Layout_t>::handle_wheel(Mouse::Button button) -> bool
{
    switch (button) {
        case Mouse::Button::ScrollDown: this->increment_position(); break;
        case Mouse::Button::ScrollUp: this->decrement_position(); break;
        default: return false;
    }
    return true;
}

template class Scrollbar<layout::Horizontal>;
template class Scrollbar<layout::Vertical>;

template <template <typename> typename Layout_t>
auto scrollbar(std::size_t size, std::size_t position)
    -> std::unique_ptr<Scrollbar<Layout_t>>
{
    return std::make_unique<Scrollbar<Layout_t>>(size, position);
}

template auto scrollbar(std::size_t, std::size_t)
    -> std::unique_ptr<HScrollbar>;
template auto scrollbar(std::size_t, std::size_t)
    -> std::unique_ptr<VScrollbar>;

template <template <typename> typename Layout_t>
auto scrollbar(typename Scrollbar<Layout_t>::Parameters p)
    -> std::unique_ptr<Scrollbar<Layout_t>>
{
    return std::make_unique<Scrollbar<Layout_t>>(std::move(p));
}

template auto scrollbar(HScrollbar::Parameters) -> std::unique_ptr<HScrollbar>;
template auto scrollbar(VScrollbar::Parameters) -> std::unique_ptr<VScrollbar>;

auto hscrollbar(std::size_t size, std::size_t position)
    -> std::unique_ptr<HScrollbar>
{
    return std::make_unique<HScrollbar>(size, position);
}

auto hscrollbar(HScrollbar::Parameters p) -> std::unique_ptr<HScrollbar>
{
    return std::make_unique<HScrollbar>(std::move(p));
}

auto vscrollbar(std::size_t size, std::size_t position)
    -> std::unique_ptr<VScrollbar>
{
    return std::make_unique<VScrollbar>(size, position);
}

auto vscrollbar(VScrollbar::Parameters p) -> std::unique_ptr<VScrollbar>
{
    return std::make_unique<VScrollbar>(std::move(p));
}

template <template <typename> typename Layout_t>
void link(Scrollbar<Layout_t>& scrollbar, Textbox& textbox)
{
    scrollbar.set_size(textbox.line_count());

    textbox.line_count_changed.connect(
        [&](std::size_t lines) { scrollbar.set_size(lines); });

    scrollbar.new_position.connect(
        [&](std::size_t p) { textbox.set_top_line(p); });

    textbox.scrolled_to.connect([&](int n) { scrollbar.set_position(n); });
}

template void link(HScrollbar&, Textbox&);
template void link(VScrollbar&, Textbox&);

}  // namespace ox
