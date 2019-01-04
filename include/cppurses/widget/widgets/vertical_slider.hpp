#ifndef CPPURSES_WIDGET_WIDGETS_VERTICAL_SLIDER_HPP
#define CPPURSES_WIDGET_WIDGETS_VERTICAL_SLIDER_HPP
#include <vector>

#include <signals/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/widget/widget.hpp>

#include "detail/slider_logic.hpp"

namespace cppurses {

class Vertical_slider : public Widget {
   public:
    /// Create a slider with [0, limit) possible values.
    Vertical_slider(std::size_t limit);

    /// Manually set the value of the slider, clamped to [0, limit).
    void set_value(std::size_t value) { logic_.set_value(value); }

    /// Retrieve the current value of the slider.
    std::size_t value() const { return logic_.value(); }

    /// Signal emitted every time the slider value has changed.
    sig::Signal<void(std::size_t)> value_changed;

   protected:
    bool paint_event() override;
    bool mouse_press_event(const Mouse_data& mouse) override;
    bool key_press_event(const Keyboard_data& keyboard) override;

   private:
    detail::Slider_logic logic_;

    Glyph upper_{L' ', background(Color::Black)};
    // std::vector<Glyph> middle_{L'░', L'▒', L'▓', L'█'};
    Glyph middle_{L' ', background(Color::White)};
    Glyph lower_{L' ', background(Color::White)};

    // std::size_t middle_index() const;
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_VERTICAL_SLIDER_HPP
