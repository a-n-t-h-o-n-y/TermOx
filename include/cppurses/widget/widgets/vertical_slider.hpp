#ifndef CPPURSES_WIDGET_WIDGETS_VERTICAL_SLIDER_HPP
#define CPPURSES_WIDGET_WIDGETS_VERTICAL_SLIDER_HPP
#include <vector>

#include <signals/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

class Vertical_slider : public Widget {
   public:
    /// Create a slider with [0, n) possible values.
    Vertical_slider(std::size_t n);

    /// Manually set the value of the slider, clamped to [0, n).
    void set_value(std::size_t value);

    /// Retrieve the position of the Slider as a value from [0, 1].
    float position() const { return position_; }

    /// Set the value to increment by on scrolling and arrow key presses.
    void set_increment(float increment) { increment_ = increment; }

    /// Retrieve the currently set increment value.
    float increment() const { return increment_; }

    /// Signal emitted every time the position has changed.
    sig::Signal<void(float)> position_changed;

   protected:
    bool paint_event() override;
    bool mouse_press_event(const Mouse_data& mouse) override;
    bool key_press_event(const Keyboard_data& keyboard) override;

   private:
    std::size_t size_n_;
    std::size_t current_value_{0};

    float position_{0.0};
    float increment_{0.005};
    Glyph upper_{L' ', background(Color::Black)};
    std::vector<Glyph> middle_{L'░', L'▒', L'▓', L'█'};
    Glyph lower_{L' ', background(Color::White)};

    std::size_t middle_index() const;
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_VERTICAL_SLIDER_HPP
