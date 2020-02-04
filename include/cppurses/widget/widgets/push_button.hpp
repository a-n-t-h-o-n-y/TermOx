#ifndef CPPURSES_WIDGET_WIDGETS_PUSH_BUTTON_HPP
#define CPPURSES_WIDGET_WIDGETS_PUSH_BUTTON_HPP
#include <utility>

#include <signals/signal.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/widget.hpp>

#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

/// Button widget that emits Signal on a left mouse button press.
class Push_button : public Widget {
   public:
    // Emitted when this Widget receives a left mouse button press event.
    sig::Signal<void()> clicked;

   public:
    /// Construct a Push_button with centered \p label.
    explicit Push_button(Glyph_string label = "") : label_{std::move(label)} {}

    /// Set the label and repaint.
    void set_label(Glyph_string label)
    {
        label_ = std::move(label);
        this->update();
    }

    /// Return the current label.
    auto label() const -> Glyph_string const& { return label_; }

    /// Return the current label.
    auto label() -> Glyph_string& { return label_; }

   protected:
    auto mouse_press_event(Mouse::State const& mouse) -> bool override
    {
        if (mouse.button == Mouse::Button::Left)
            clicked();
        return Widget::mouse_press_event(mouse);
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

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_PUSH_BUTTON_HPP
