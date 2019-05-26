#ifndef CPPURSES_WIDGET_WIDGETS_PUSH_BUTTON_HPP
#define CPPURSES_WIDGET_WIDGETS_PUSH_BUTTON_HPP
#include <signals/signal.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

/// Button widget that emits Signal on a left mouse button press.
class Push_button : public Widget {
   public:
    /// Construct a Push_button with centered \p label.
    explicit Push_button(Glyph_string label = "");

    /// Set the label and repaint.
    void set_label(Glyph_string label);

    /// Return the current label.
    const Glyph_string& label() const { return label_; }

    /// Return the current label.
    Glyph_string& label() { return label_; }

    // Emitted when this Widget receives a left mouse button press event.
    sig::Signal<void()> clicked;

   protected:
    bool mouse_press_event(const Mouse::State& mouse) override;
    bool paint_event() override;

   private:
    Glyph_string label_;
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_PUSH_BUTTON_HPP
