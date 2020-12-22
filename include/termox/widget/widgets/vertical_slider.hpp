#ifndef TERMOX_WIDGET_WIDGETS_VERTICAL_SLIDER_HPP
#define TERMOX_WIDGET_WIDGETS_VERTICAL_SLIDER_HPP
#include <vector>

#include <signals_light/signal.hpp>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/focus_policy.hpp>
#include <termox/widget/widget.hpp>

#include "detail/slider_logic.hpp"

namespace ox {

class Vertical_slider : public Widget {
   public:
    using Value_t = detail::Slider_logic::Value_t;
    using Ratio_t = detail::Slider_logic::Ratio_t;

    /// Signal emitted every time the slider value has changed.
    sl::Signal<void(Value_t)>& value_changed = logic_.value_changed;

    /// Signal emitted every time the slider ratio has changed.
    sl::Signal<void(Ratio_t)>& ratio_changed = logic_.ratio_changed;

   public:
    /// Create a slider with [minimum, maximum] possible values.
    /** \p minimum and \p maximum are undefined if maximum <= minimum. */
    Vertical_slider(Value_t minimum, Value_t maximum) : logic_{minimum, maximum}
    {
        this->focus_policy = Focus_policy::Strong;
    }

    /// Manually set the value of the slider, clamped to [minimum, maximum].
    void set_value(Value_t value)
    {
        logic_.set_value(value);
        this->update();
    }

    /// Manually set the ratio of the slider, clamped to [0.0, 1.0].
    void set_ratio(Ratio_t ratio)
    {
        logic_.set_ratio(ratio);
        this->update();
    }

    /// Increment the Slider's value by \p amount.
    void increment(Value_t amount = 1)
    {
        inverted_ ? logic_.decrement(amount) : logic_.increment(amount);
        this->update();
    }

    /// Decrement the Slider's value by \p amount.
    void decrement(Value_t amount = 1)
    {
        inverted_ ? logic_.increment(amount) : logic_.decrement(amount);
        this->update();
    }

    /// Set the minimum value the slider can be at.
    /** Modifies maximum to be min + 1 if maximum <= min. */
    void set_minimum(Value_t min)
    {
        logic_.set_minimum(min);
        this->update();
    }

    /// Set the maximum value the slider can be at.
    /** Modifies minimum to be max - 1 if minimum >= max. */
    void set_maximum(Value_t max)
    {
        logic_.set_maximum(max);
        this->update();
    }

    /// Return the current value of the slider.
    auto value() const -> Value_t { return logic_.value(); }

    /// Return the smallest possible value for the slider.
    auto minimum() const -> Value_t { return logic_.minimum(); }

    /// Return the largest possible value for the slider.
    auto maximum() const -> Value_t { return logic_.maximum(); }

    /// Flip the Slider so the minimum value is at the top of the display.
    /** Upper and lower Glyph_strings are also flipped. */
    void invert(bool enable = true)
    {
        inverted_ = enable;
        this->update();
    };

    /// Return if slider is inverted.
    auto is_inverted() const -> bool { return inverted_; }

    /// Set the upper glyph represetation.
    /** String is printed starting one above the indicator and is repeated if \p
     *  repeats is true. */
    void set_upper(Glyph_string const& symbols, bool repeats = true)
    {
        upper_         = symbols;
        upper_repeats_ = repeats;
        this->update();
    }

    /// Set the middle indicator's representation.
    /** \p symbols is iterated through to display one Glyph at a time, dependent
     *  on the current ratio and y position of the indicator. */
    void set_indicator(Glyph_string const& symbols)
    {
        indicator_ = symbols;
        this->update();
    }

    /// Set the lower glyph represetation.
    /** String is printed starting one below the indicator and is repeated if \p
     *  repeats is true. */
    void set_lower(Glyph_string const& symbols, bool repeats = true)
    {
        lower_         = symbols;
        lower_repeats_ = repeats;
        this->update();
    }

    /// Return the upper representation of the Slider.
    auto upper() const -> Glyph_string const& { return upper_; }

    /// Return the indicator's representation.
    auto indicator() const -> Glyph_string const& { return indicator_; }

    /// Return the lower representation of the Slider.
    auto lower() const -> Glyph_string const& { return lower_; }

   protected:
    auto paint_event() -> bool override;

    auto mouse_press_event(Mouse const& m) -> bool override;

    auto mouse_wheel_event(Mouse const& m) -> bool override;

    auto key_press_event(Key k) -> bool override
    {
        switch (k) {
            case Key::Arrow_up: this->increment(); break;
            case Key::Arrow_down: this->decrement(); break;
            default: break;
        }
        return Widget::key_press_event(k);
    }

   private:
    detail::Slider_logic logic_;
    bool inverted_ = false;

    // candidate for struct
    Glyph_string upper_     = {" ", bg(Color::Black)};
    bool upper_repeats_     = true;
    Glyph_string indicator_ = "░▒▓█";
    Glyph_string lower_     = {" ", bg(Color::White)};
    bool lower_repeats_     = true;

   private:
    /// Calculate the ratio of the given y position to the height of *this.
    /** Inverted so that height() - 1 is a ratio of 0.0 and position 0 is a
     *  ratio of 1.0. */
    auto ratio_at(std::size_t position) const -> Ratio_t;

    /// Return the indicator's position without respect to its invertedness.
    auto indicator_absolute_position() const -> Ratio_t;

    /// Get the indicator's y position on the screen from the current ratio.
    auto indicator_position() const -> std::size_t;

    /// Find the index into the middle_ container that cooresponds to the ratio.
    auto indicator_index() const -> std::size_t;
};

/// Helper function to create an instance.
template <typename... Args>
auto vertical_slider(Args&&... args) -> std::unique_ptr<Vertical_slider>
{
    return std::make_unique<Vertical_slider>(std::forward<Args>(args)...);
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_VERTICAL_SLIDER_HPP
