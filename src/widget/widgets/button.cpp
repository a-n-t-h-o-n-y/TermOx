#include <termox/widget/widgets/button.hpp>

#include <memory>
#include <utility>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/painter/painter.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>

namespace ox {

Button::Button(Glyph_string text) : text_{std::move(text)} {}

Button::Button(Parameters p) : text_{std::move(p.text)} {}

void Button::set_text(Glyph_string text)
{
    text_ = std::move(text);
    this->update();
}

auto Button::text() const -> Glyph_string const& { return text_; }

auto Button::mouse_press_event(Mouse const& m) -> bool
{
    if (m.button == Mouse::Button::Left)
        this->pressed();
    return Widget::mouse_press_event(m);
}

auto Button::mouse_release_event(Mouse const& m) -> bool
{
    if (m.button == Mouse::Button::Left)
        this->released();
    return Widget::mouse_release_event(m);
}

auto Button::paint_event(Painter& p) -> bool
{
    auto const width = this->area().width;
    auto const x     = text_.size() > width ? 0 : (width - text_.size()) / 2;
    auto const y     = this->area().height / 2;
    p.put(text_, {x, y});
    return Widget::paint_event(p);
}

auto button(Glyph_string text) -> std::unique_ptr<Button>
{
    return std::make_unique<Button>(std::move(text));
}

auto button(Button::Parameters p) -> std::unique_ptr<Button>
{
    return std::make_unique<Button>(std::move(p));
}

Push_button::Push_button(Glyph_string text,
                         Color pressed_color,
                         Color released_color)
    : Button{std::move(text)},
      pressed_color_{pressed_color},
      released_color_{released_color}
{
    *this | bg(released_color_);
    this->Button::pressed.connect([this] { *this | bg(pressed_color_); });
    this->Button::released.connect([this] { *this | bg(released_color_); });
}

Push_button::Push_button(Parameters p)
    : Push_button{std::move(p.text), p.pressed_color, p.released_color}
{}

void Push_button::set_pressed_color(Color c) { pressed_color_ = c; }

auto Push_button::get_pressed_color() const -> Color { return pressed_color_; };

void Push_button::set_released_color(Color c)
{
    released_color_ = c;
    *this | bg(released_color_);
}

auto Push_button::get_released_color() const -> Color
{
    return released_color_;
}

auto push_button(Glyph_string text, Color pressed, Color released)
    -> std::unique_ptr<Push_button>
{
    return std::make_unique<Push_button>(std::move(text), pressed, released);
}

auto push_button(Push_button::Parameters p) -> std::unique_ptr<Push_button>
{
    return std::make_unique<Push_button>(std::move(p));
}

template <template <typename> typename Layout_t>
Thin_button<Layout_t>::Thin_button(Glyph_string text) : Button{std::move(text)}
{
    if constexpr (layout::is_vertical_v<Layout_t<Widget>>)
        *this | pipe::fixed_width(1);
    else
        *this | pipe::fixed_height(1);
}

template <template <typename> typename Layout_t>
Thin_button<Layout_t>::Thin_button(Parameters p)
    : Thin_button{std::move(p.text)}
{}

// Explicit Instantiation
template class Thin_button<layout::Vertical>;
template class Thin_button<layout::Horizontal>;

template <template <typename> typename Layout_t>
auto thin_button(Glyph_string text) -> std::unique_ptr<Thin_button<Layout_t>>
{
    return std::make_unique<Thin_button<Layout_t>>(std::move(text));
}

// Explicit Instantiation
template auto thin_button(Glyph_string) -> std::unique_ptr<HThin_button>;
template auto thin_button(Glyph_string) -> std::unique_ptr<VThin_button>;

template <template <typename> typename Layout_t>
auto thin_button(typename Thin_button<Layout_t>::Parameters p)
    -> std::unique_ptr<Thin_button<Layout_t>>
{
    return std::make_unique<Thin_button<Layout_t>>(std::move(p));
}

// Explicit Instantiation
template auto thin_button(HThin_button::Parameters)
    -> std::unique_ptr<HThin_button>;

template auto thin_button(VThin_button::Parameters)
    -> std::unique_ptr<VThin_button>;

auto hthin_button(Glyph_string text) -> std::unique_ptr<HThin_button>
{
    return std::make_unique<HThin_button>(std::move(text));
}

auto hthin_button(HThin_button::Parameters p) -> std::unique_ptr<HThin_button>
{
    return std::make_unique<HThin_button>(std::move(p));
}

auto vthin_button(Glyph_string text) -> std::unique_ptr<VThin_button>
{
    return std::make_unique<VThin_button>(std::move(text));
}

auto vthin_button(VThin_button::Parameters p) -> std::unique_ptr<VThin_button>
{
    return std::make_unique<VThin_button>(std::move(p));
}

}  // namespace ox
