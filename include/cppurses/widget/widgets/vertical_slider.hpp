#ifndef CPPURSES_WIDGET_WIDGETS_VERTICAL_SLIDER_HPP
#define CPPURSES_WIDGET_WIDGETS_VERTICAL_SLIDER_HPP
#include <vector>

#include <signals/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/events/key.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/widget.hpp>

#include "detail/slider_logic.hpp"

namespace cppurses {

class Vertical_slider : public Widget {
    detail::Slider_logic logic_;

   public:
    using Value_t = detail::Slider_logic::Value_t;
    using Ratio_t = detail::Slider_logic::Ratio_t;

    /// Create a slider with [minimum, maximum] possible values.
    /** \p minimum and \p maximum are undefined if maximum <= minimum. */
    Vertical_slider(Value_t minimum, Value_t maximum);

    /// Manually set the value of the slider, clamped to [minimum, maximum].
    void set_value(Value_t value);

    /// Manually set the ratio of the slider, clamped to [0.0, 1.0].
    void set_ratio(Ratio_t ratio);

    /// Increment the Slider's value by \p amount.
    void increment(Value_t amount = 1);

    /// Decrement the Slider's value by \p amount.
    void decrement(Value_t amount = 1);

    /// Set the minimum value the slider can be at.
    /** Modifies maximum to be min + 1 if maximum <= min. */
    void set_minimum(Value_t min);

    /// Set the maximum value the slider can be at.
    /** Modifies minimum to be max - 1 if minimum >= max. */
    void set_maximum(Value_t max);

    /// Return the current value of the slider.
    Value_t value() const { return logic_.value(); }

    /// Return the smallest possible value for the slider.
    Value_t minimum() const { return logic_.minimum(); }

    /// Return the largest possible value for the slider.
    Value_t maximum() const { return logic_.maximum(); }

    /// Flip the Slider so the minimum value is at the top of the display.
    /** Upper and lower Glyph_strings are also flipped. */
    void invert(bool enable = true) {
        inverted_ = enable;
        this->update();
    };

    /// Return if slider is inverted.
    bool is_inverted() const { return inverted_; }

    /// Set the upper glyph represetation.
    /** String is printed starting one above the indicator and is repeated if \p
     *  repeats is true. */
    void set_upper(const Glyph_string& symbols, bool repeats = true);

    /// Set the middle indicator's representation.
    /** \p symbols is iterated through to display one Glyph at a time, dependent
     *  on the current ratio and y position of the indicator. */
    void set_indicator(const Glyph_string& symbols);

    /// Set the lower glyph represetation.
    /** String is printed starting one below the indicator and is repeated if \p
     *  repeats is true. */
    void set_lower(const Glyph_string& symbols, bool repeats = true);

    /// Return the upper representation of the Slider.
    const Glyph_string& upper() const { return upper_; }

    /// Return the indicator's representation.
    const Glyph_string& indicator() const { return indicator_; }

    /// Return the lower representation of the Slider.
    const Glyph_string& lower() const { return lower_; }

    /// Signal emitted every time the slider value has changed.
    sig::Signal<void(Value_t)>& value_changed{logic_.value_changed};

    /// Signal emitted every time the slider ratio has changed.
    sig::Signal<void(Ratio_t)>& ratio_changed{logic_.ratio_changed};

   protected:
    bool paint_event() override;
    bool mouse_press_event(const Mouse::State& mouse) override;
    bool key_press_event(const Key::State& keyboard) override;

   private:
    // candidate for struct
    Glyph_string upper_{" ", background(Color::Black)};
    bool upper_repeats_{true};
    Glyph_string indicator_{"░▒▓█"};
    Glyph_string lower_{" ", background(Color::White)};
    bool lower_repeats_{true};

    bool inverted_{false};

    /// Calculate the ratio of the given y position to the height of *this.
    /** Inverted so that height() - 1 is a ratio of 0.0 and position 0 is a
     *  ratio of 1.0. */
    Ratio_t ratio_at(std::size_t position) const;

    /// Return the indicator's position without respect to its invertedness.
    Ratio_t indicator_absolute_position() const;

    /// Get the indicator's y position on the screen from the current ratio.
    std::size_t indicator_position() const;

    /// Find the index into the middle_ container that cooresponds to the ratio.
    std::size_t indicator_index() const;
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_VERTICAL_SLIDER_HPP
