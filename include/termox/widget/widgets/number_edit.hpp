#ifndef TERMOX_WIDGET_WIDGETS_NUMBER_EDIT_HPP
#define TERMOX_WIDGET_WIDGETS_NUMBER_EDIT_HPP
#include <algorithm>
#include <cctype>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>

#include <signals_light/signal.hpp>

#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/align.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widgets/detail/textline_base.hpp>

namespace ox {

template <typename Number_t>
class Number_edit : public detail::Textline_base {
   public:
    struct Range {
        Number_t min = Min_num;
        Number_t max = Max_num;
    };

    struct Parameters {
        Number_t initial       = 0;
        Range range            = {};
        Align alignment        = Align::Left;
        Action focus_in_action = Action::None;
        Number_t increment     = 1;
    };

   public:
    /// Emitted on Enter key press; sends along the current value.
    sl::Signal<void(Number_t)> submitted;

    /// Emitted on key press and inc/decrement; sends along the current value.
    sl::Signal<void(Number_t)> value_modified;

   public:
    explicit Number_edit(Number_t initial       = 0,
                         Range range            = {},
                         Align alignment        = Align::Left,
                         Action focus_in_action = Action::None,
                         Number_t increment     = 1)
        : Textline_base{std::to_string(
                            std::clamp(initial, range.min, range.max)),
                        alignment, focus_in_action},
          value_{std::clamp(initial, range.min, range.max)},
          range_{range},
          increment_{increment >= 0 ? increment : 0}
    {}

    explicit Number_edit(Parameters p)
        : Number_edit{p.initial, p.range, p.alignment, p.focus_in_action,
                      p.increment}
    {}

   public:
    /// Set a new value to be displayed.
    /** Does not emit Signals. Clamps to range if outside of it. */
    void set_value(Number_t x)
    {
        value_ = std::clamp(x, range_.min, range_.max);
        core_.set_text(std::to_string(value_));
        this->update();
    }

    /// Get the current value of the Number_edit, defaults to zero if invalid.
    [[nodiscard]] auto value() const -> Number_t { return value_; }

    /// Set the accepted range of values
    void set_range(Range x)
    {
        range_ = x;
        this->set_value(value_);  // clamp to new range
    }

    [[nodiscard]] auto range() const -> Range { return range_; }

    /// Set the value to increment/decrement by on arrow key/mouse wheel input.
    void set_increment(Number_t x) { increment_ = x; }

    /// Return the current increment/decrement value.
    [[nodiscard]] auto get_increment() const -> Number_t { return increment_; }

    /// Increment the current value by the set increment.
    void increment()
    {
        this->set_value(
            Max_num - increment_ < value_  // Overflow Check
                ? range_.max
                : std::clamp(value_ + increment_, range_.min, range_.max));
        value_modified.emit(value_);
        submitted.emit(value_);
    }

    /// Decrement the current value by the set increment.
    void decrement()
    {
        this->set_value(
            Min_num + increment_ > value_  // Underflow Check
                ? range_.min
                : std::clamp(value_ - increment_, range_.min, range_.max));
        value_modified.emit(value_);
        submitted.emit(value_);
    }

   protected:
    auto key_press_event(Key k) -> bool override
    {
        switch (k) {
            case Key::Enter: this->validate_and_submit(); break;
            case Key::Arrow_up: this->increment(); break;
            case Key::Arrow_down: this->decrement(); break;
            default: break;
        }
        auto const c = key_to_char(k);
        if (is_valid_input(c)) {
            core_.insert_at_cursor(c);
            this->update();
            value_ = string_to_value(this->text().str());
            value_modified.emit(value_);
        }
        return Textline_base::key_press_event(k);
    }

    auto mouse_wheel_event(Mouse const& m) -> bool override
    {
        switch (m.button) {
            case Mouse::Button::ScrollUp: this->increment(); break;
            case Mouse::Button::ScrollDown: this->decrement(); break;
            default: break;
        }
        return Textline_base::mouse_wheel_event(m);
    }

    auto focus_out_event() -> bool override
    {
        this->validate_and_submit();
        return Textline_base::focus_out_event();
    }

   private:
    Number_t value_;
    Range range_;
    Number_t increment_;

    static auto constexpr Min_num = std::numeric_limits<Number_t>::lowest();
    static auto constexpr Max_num = std::numeric_limits<Number_t>::max();

   private:
    /// Verifies if \p c is a valid input for Number_t.
    [[nodiscard]] static auto is_valid_input(char c) -> bool
    {
        bool is_separator = (c == ',');
        if constexpr (std::is_floating_point<Number_t>::value)
            is_separator |= (c == '.');
        bool is_sign = (c == '+');
        if constexpr (!std::is_unsigned<Number_t>::value)
            is_sign |= (c == '-');
        return std::isdigit(c) || is_separator || is_sign;
    }

    /// transform a string into a Number_t value.
    [[nodiscard]] auto string_to_value(std::string x) -> Number_t
    {
        sanitize(x);
        auto ss     = std::istringstream{x};
        auto result = Number_t(0);
        ss >> result;
        return result;
    }

    /// Remove commas from string, in place.
    static void sanitize(std::string& s)
    {
        s.erase(std::remove(std::begin(s), std::end(s), ','), std::end(s));
    }

    /// Clamps the current string value to the range and emits submitted Signal.
    void validate_and_submit()
    {
        auto const new_value = string_to_value(this->text().str());
        value_               = std::clamp(new_value, range_.min, range_.max);
        if (value_ != new_value) {
            core_.set_text(std::to_string(value_));
            this->update();
        }
        submitted.emit(value_);
    }
};

/// Helper function to create a Number_edit instance.
template <typename Number_t>
[[nodiscard]] auto number_edit(
    Number_t initial                            = 0,
    typename Number_edit<Number_t>::Range range = {},
    Align alignment                             = Align::Left,
    typename Number_edit<Number_t>::Action focus_in_action =
        Number_edit<Number_t>::Action::None,
    Number_t increment = 1) -> std::unique_ptr<Number_edit<Number_t>>
{
    return std::make_unique<Number_edit<Number_t>>(initial, range, alignment,
                                                   focus_in_action, increment);
}

/// Helper function to create a Number_edit instance.
template <typename Number_t>
[[nodiscard]] auto number_edit(typename Number_edit<Number_t>::Parameters p)
    -> std::unique_ptr<Number_edit<Number_t>>
{
    return std::make_unique<Number_edit<Number_t>>(std::move(p));
}

using Int_edit = Number_edit<int>;

/// Helper function to create an Int_edit instance.
[[nodiscard]] inline auto int_edit(
    int initial                      = 0,
    Int_edit::Range range            = {},
    Align alignment                  = Align::Left,
    Int_edit::Action focus_in_action = Int_edit::Action::None,
    int increment                    = 1) -> std::unique_ptr<Int_edit>
{
    return std::make_unique<Int_edit>(initial, range, alignment,
                                      focus_in_action, increment);
}

/// Helper function to create an Int_edit instance.
[[nodiscard]] inline auto int_edit(Int_edit::Parameters p)
    -> std::unique_ptr<Int_edit>
{
    return std::make_unique<Int_edit>(std::move(p));
}

using Unsigned_edit = Number_edit<unsigned int>;

/// Helper function to create an Unsigned_edit instance.
[[nodiscard]] inline auto unsigned_edit(
    unsigned int initial                  = 0,
    Unsigned_edit::Range range            = {},
    Align alignment                       = Align::Left,
    Unsigned_edit::Action focus_in_action = Unsigned_edit::Action::None,
    unsigned int increment                = 1) -> std::unique_ptr<Unsigned_edit>
{
    return std::make_unique<Unsigned_edit>(initial, range, alignment,
                                           focus_in_action, increment);
}

/// Helper function to create an Unsigned_edit instance.
[[nodiscard]] inline auto unsigned_edit(Unsigned_edit::Parameters p)
    -> std::unique_ptr<Unsigned_edit>
{
    return std::make_unique<Unsigned_edit>(std::move(p));
}

using Double_edit = Number_edit<double>;

/// Helper function to create a Double_edit instance.
[[nodiscard]] inline auto double_edit(
    double initial                      = 0,
    Double_edit::Range range            = {},
    Align alignment                     = Align::Left,
    Double_edit::Action focus_in_action = Double_edit::Action::None,
    double increment                    = 1) -> std::unique_ptr<Double_edit>
{
    return std::make_unique<Double_edit>(initial, range, alignment,
                                         focus_in_action, increment);
}

/// Helper function to create a Double_edit instance.
[[nodiscard]] inline auto double_edit(Double_edit::Parameters p)
    -> std::unique_ptr<Double_edit>
{
    return std::make_unique<Double_edit>(std::move(p));
}

using Float_edit = Number_edit<float>;

/// Helper function to create a Float_edit instance.
[[nodiscard]] inline auto float_edit(
    float initial                      = 0,
    Float_edit::Range range            = {},
    Align alignment                    = Align::Left,
    Float_edit::Action focus_in_action = Float_edit::Action::None,
    float increment                    = 1) -> std::unique_ptr<Float_edit>
{
    return std::make_unique<Float_edit>(initial, range, alignment,
                                        focus_in_action, increment);
}

/// Helper function to create a Float_edit instance.
[[nodiscard]] inline auto float_edit(Float_edit::Parameters p)
    -> std::unique_ptr<Float_edit>
{
    return std::make_unique<Float_edit>(std::move(p));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_NUMBER_EDIT_HPP
