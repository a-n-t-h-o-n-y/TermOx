#ifndef TERMOX_WIDGET_WIDGETS_BUTTON_HPP
#define TERMOX_WIDGET_WIDGETS_BUTTON_HPP
#include <memory>
#include <utility>

#include <signals_light/signal.hpp>

#include <termox/painter/glyph_string.hpp>
#include <termox/painter/painter.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

/// Button widget that emits Signal on a left mouse button press.
class Button : public Widget {
   public:
    using Parameters = Glyph_string;

   public:
    // Emitted when this Widget receives a left mouse button press event.
    sl::Signal<void()> pressed;
    sl::Signal<void()> released;

   public:
    /// Construct a Button with centered \p label.
    explicit Button(Glyph_string label = L"") : label_{std::move(label)} {}

   public:
    /// Set the label and repaint.
    void set_label(Glyph_string label)
    {
        label_ = std::move(label);
        this->update();
    }

    /// Return the current label.
    auto get_label() const -> Glyph_string const& { return label_; }

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

    auto paint_event() -> bool override
    {
        auto const width = this->width();
        auto const x = label_.size() > width ? 0 : (width - label_.size()) / 2;
        auto const y = this->height() / 2;
        Painter{*this}.put(label_, x, y);
        return Widget::paint_event();
    }

   private:
    Glyph_string label_;
};

/// Helper function to create a Button instance.
template <typename... Args>
auto button(Args&&... args) -> std::unique_ptr<Button>
{
    return std::make_unique<Button>(std::forward<Args>(args)...);
}

/// Button that changes its background Color on press and release events.
class Push_button : public Button {
   public:
    struct Parameters {
        Glyph_string label;
        Color pressed  = Color::Foreground;
        Color released = Color::Background;
    };

   public:
    /// Construct a new Push_button
    explicit Push_button(Glyph_string label = L"",
                         Color pressed      = Color::Foreground,
                         Color released     = Color::Background)
        : Button{std::move(label)},
          pressed_color_{pressed},
          released_color_{released}
    {
        *this | bg(released_color_);
        this->Button::pressed.connect([this] { *this | bg(pressed_color_); });
        this->Button::released.connect([this] { *this | bg(released_color_); });
    }

    /// Construct a new Push_button
    explicit Push_button(Parameters p)
        : Push_button{std::move(p.label), p.pressed, p.released}
    {}

   public:
    /// Set the background Color of the Button when pressed Signal is emitted.
    void set_pressed_color(Color c) { pressed_color_ = c; }

    /// Return the current Color assigned to mouse press events.
    auto get_pressed_color() const -> Color { return pressed_color_; };

    /// Set the background Color of the Button when released Signal is emitted.
    void set_released_color(Color c)
    {
        released_color_ = c;
        *this | bg(released_color_);
    }

    /// Return the current Color assigned to mouse release events.
    auto get_released_color() const -> Color { return released_color_; };

   private:
    Color pressed_color_;
    Color released_color_;
};

/// Helper function to create a Push_button instance.
template <typename... Args>
auto push_button(Args&&... args) -> std::unique_ptr<Push_button>
{
    return std::make_unique<Push_button>(std::forward<Args>(args)...);
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_BUTTON_HPP
