#ifndef TERMOX_WIDGET_WIDGETS_BUTTON_HPP
#define TERMOX_WIDGET_WIDGETS_BUTTON_HPP
#include <memory>
#include <utility>

#include <signals_light/signal.hpp>

#include <termox/painter/glyph_string.hpp>
#include <termox/painter/painter.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

/// Button widget that emits Signal on a left mouse button press.
class Button : public Widget {
   public:
    struct Parameters {
        Glyph_string text;
    };

   public:
    // Emitted when this Widget receives a left mouse button press event.
    sl::Signal<void()> pressed;
    sl::Signal<void()> released;

   public:
    /// Construct a Button with centered \p text.
    explicit Button(Glyph_string text = U"") : text_{std::move(text)} {}

    explicit Button(Parameters parameters) : text_{std::move(parameters.text)}
    {}

   public:
    /// Set the text and repaint.
    void set_text(Glyph_string text)
    {
        text_ = std::move(text);
        this->update();
    }

    /// Return the current text.
    [[nodiscard]] auto text() const -> Glyph_string const& { return text_; }

   protected:
    auto mouse_press_event(Mouse const& m) -> bool override
    {
        if (m.button == Mouse::Button::Left)
            this->pressed();
        return Widget::mouse_press_event(m);
    }

    auto mouse_release_event(Mouse const& m) -> bool override
    {
        if (m.button == Mouse::Button::Left)
            this->released();
        return Widget::mouse_release_event(m);
    }

    auto paint_event(Painter& p) -> bool override
    {
        auto const width = this->width();
        auto const x = text_.size() > width ? 0 : (width - text_.size()) / 2;
        auto const y = this->height() / 2;
        p.put(text_, {x, y});
        return Widget::paint_event(p);
    }

   private:
    Glyph_string text_;
};

/// Helper function to create a Button instance.
[[nodiscard]] inline auto button(Glyph_string text = U"")
    -> std::unique_ptr<Button>
{
    return std::make_unique<Button>(std::move(text));
}

/// Helper function to create a Button instance.
[[nodiscard]] inline auto button(Button::Parameters p = {})
    -> std::unique_ptr<Button>
{
    return std::make_unique<Button>(std::move(p));
}

/// Button that changes its background Color on press and release events.
class Push_button : public Button {
   public:
    struct Parameters {
        Glyph_string text;
        Color pressed  = Color::Foreground;
        Color released = Color::Background;
    };

   public:
    /// Construct a new Push_button
    explicit Push_button(Glyph_string text = U"",
                         Color pressed     = Color::Foreground,
                         Color released    = Color::Background)
        : Button{std::move(text)},
          pressed_color_{pressed},
          released_color_{released}
    {
        *this | bg(released_color_);
        this->Button::pressed.connect([this] { *this | bg(pressed_color_); });
        this->Button::released.connect([this] { *this | bg(released_color_); });
    }

    /// Construct a new Push_button
    explicit Push_button(Parameters p)
        : Push_button{std::move(p.text), p.pressed, p.released}
    {}

   public:
    /// Set the background Color of the Button when pressed Signal is emitted.
    void set_pressed_color(Color c) { pressed_color_ = c; }

    /// Return the current Color assigned to mouse press events.
    [[nodiscard]] auto get_pressed_color() const -> Color
    {
        return pressed_color_;
    };

    /// Set the background Color of the Button when released Signal is emitted.
    void set_released_color(Color c)
    {
        released_color_ = c;
        *this | bg(released_color_);
    }

    /// Return the current Color assigned to mouse release events.
    [[nodiscard]] auto get_released_color() const -> Color
    {
        return released_color_;
    };

   private:
    Color pressed_color_;
    Color released_color_;
};

/// Helper function to create a Push_button instance.
[[nodiscard]] inline auto push_button(Glyph_string text = U"",
                                      Color pressed     = Color::Foreground,
                                      Color released    = Color::Background)
    -> std::unique_ptr<Push_button>
{
    return std::make_unique<Push_button>(std::move(text), pressed, released);
}

/// Helper function to create a Push_button instance.
[[nodiscard]] inline auto push_button(Push_button::Parameters p = {})
    -> std::unique_ptr<Push_button>
{
    return std::make_unique<Push_button>(std::move(p));
}

/// Width or Height 1 Button
template <template <typename> typename Layout_t>
class Thin_button : public Button {
   public:
    using Button::Parameters;

   public:
    explicit Thin_button(Glyph_string text = U"") : Button{std::move(text)}
    {
        if constexpr (layout::is_vertical_v<Layout_t<Widget>>)
            *this | pipe::fixed_width(1);
        else
            *this | pipe::fixed_height(1);
    }

    explicit Thin_button(Parameters parameters)
        : Thin_button{std::move(parameters.text)}
    {}
};

/// Helper function to create a Thin_button instance.
template <template <typename> typename Layout_t>
[[nodiscard]] auto thin_button(Glyph_string text = U"")
    -> std::unique_ptr<Thin_button<Layout_t>>
{
    return std::make_unique<Thin_button<Layout_t>>(std::move(text));
}

/// Helper function to create a Thin_button instance.
template <template <typename> typename Layout_t>
[[nodiscard]] auto thin_button(
    typename Thin_button<Layout_t>::Parameters p = {})
    -> std::unique_ptr<Thin_button<Layout_t>>
{
    return std::make_unique<Thin_button<Layout_t>>(std::move(p));
}

using HThin_button = Thin_button<layout::Horizontal>;

/// Helper function to create an HThin_button instance.
[[nodiscard]] inline auto hthin_button(Glyph_string text = U"")
    -> std::unique_ptr<HThin_button>
{
    return std::make_unique<HThin_button>(std::move(text));
}

/// Helper function to create an HThin_button instance.
[[nodiscard]] inline auto hthin_button(typename HThin_button::Parameters p = {})
    -> std::unique_ptr<HThin_button>
{
    return std::make_unique<HThin_button>(std::move(p));
}

using VThin_button = Thin_button<layout::Vertical>;

/// Helper function to create a VThin_button instance.
[[nodiscard]] inline auto vthin_button(Glyph_string text = U"")
    -> std::unique_ptr<VThin_button>
{
    return std::make_unique<VThin_button>(std::move(text));
}

/// Helper function to create a VThin_button instance.
[[nodiscard]] inline auto vthin_button(typename VThin_button::Parameters p = {})
    -> std::unique_ptr<VThin_button>
{
    return std::make_unique<VThin_button>(std::move(p));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_BUTTON_HPP
