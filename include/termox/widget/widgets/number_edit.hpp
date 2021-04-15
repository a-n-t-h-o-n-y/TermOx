#ifndef TERMOX_WIDGET_WIDGETS_NUMBER_EDIT_HPP
#define TERMOX_WIDGET_WIDGETS_NUMBER_EDIT_HPP
#include <cctype>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>

#include <signals_light/signal.hpp>

#include <termox/system/mouse.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widgets/label.hpp>
#include <termox/widget/widgets/textline.hpp>

namespace ox {

// TODO Add Alignment as param once user input works with Align in Textline.

/// A Textline for specified number types.
/** Provides validator specific to Number_t. Uses std::stringstream to convert
 *  from string to Number_t value. */
template <typename Number_t = int>
class Number_edit : public Textline {
   public:
    struct Parameters {
        Number_t value = 0;
    };

   public:
    /// Emitted on Enter Key press, sends along the current value.
    sl::Signal<void(Number_t)> value_set;

   public:
    /// Construct a Number_edit with \p initial value.
    explicit Number_edit(Number_t initial = 0)
        : Textline{std::to_string(initial)}, value_{initial}
    {
        this->set_validator([](char c) { return is_valid_input(c); });
        this->enter_pressed.connect([this](std::string const& text) {
            auto ss     = std::stringstream{text};
            auto result = Number_t(0);
            ss >> result;
            value_set.emit(result);
        });
        this->set_value(initial);
    }

    explicit Number_edit(Parameters parameters) : Number_edit{parameters.value}
    {}

   public:
    /// Manually set the value of the Number_edit.
    /** Does not emit a value_set Signal. */
    void set_value(Number_t value)
    {
        value_ = value;
        this->display_value();
    }

    /// Return the currently set value.
    [[nodiscard]] auto value() const -> Number_t { return value_; }

   protected:
    auto mouse_wheel_event(Mouse const& m) -> bool override
    {
        switch (m.button) {
            case Mouse::Button::ScrollUp: this->increment(); break;
            case Mouse::Button::ScrollDown: this->decrement(); break;
            default: break;
        }
        return Textline::mouse_wheel_event(m);
    }

   private:
    Number_t value_;

   private:
    /// Verifies if \p c is a valid input for Number_t.
    [[nodiscard]] static auto is_valid_input(char c) -> bool
    {
        auto is_separator = false;
        if constexpr (std::is_floating_point<Number_t>::value)
            is_separator = (c == '.' || c == ',');
        auto is_sign = (c == '+');
        if constexpr (!std::is_unsigned<Number_t>::value)
            is_sign |= (c == '-');
        return std::isdigit(c) || is_separator || is_sign;
    }

    void display_value()
    {
        auto ss = std::stringstream{};
        ss << value_;
        this->set_contents(ss.str());
    }

    void emit_signal() { value_set.emit(value_); }

    void increment()
    {
        if (value_ == std::numeric_limits<Number_t>::max())
            return;
        ++value_;
        this->display_value();
        this->emit_signal();
    }

    void decrement()
    {
        if (value_ == std::numeric_limits<Number_t>::min())
            return;
        --value_;
        this->display_value();
        this->emit_signal();
    }
};

/// Helper function to create a Number_edit instance.
template <typename Number_t = int>
[[nodiscard]] auto number_edit(Number_t initial = 0)
    -> std::unique_ptr<Number_edit<Number_t>>
{
    return std::make_unique<Number_edit<Number_t>>(initial);
}

/// Helper function to create a Number_edit instance.
template <typename Number_t = int>
[[nodiscard]] auto number_edit(
    typename Number_edit<Number_t>::Parameters parameters)
    -> std::unique_ptr<Number_edit<Number_t>>
{
    return std::make_unique<Number_edit<Number_t>>(std::move(parameters));
}

/// Number_edit with preceding Label arranged horizontally.
template <typename Number_t = int>
class Labeled_number_edit : public HLabel_left<Number_edit<Number_t>> {
   private:
    using Base = HLabel_left<Number_edit<Number_t>>;

   public:
    struct Parameters {
        Glyph_string title;
        Number_t initial;
    };

   public:
    HLabel& label                      = Base::label;
    Number_edit<Number_t>& number_edit = Base::wrapped;

   public:
    /// Emitted on Enter Key press, sends along the current value.
    sl::Signal<void(Number_t)>& value_set = number_edit.value_set;

   public:
    /// Construct with \p title for Label text and \p initial value.
    Labeled_number_edit(Glyph_string title, Number_t initial)
        : Base({std::move(title)}, initial)
    {
        using namespace pipe;
        *this | fixed_height(1) | direct_focus();
        label | dynamic_growth();
        number_edit | bg(Color::White) | fg(Color::Black) | ghost(Color::Gray);
    }

    explicit Labeled_number_edit(Parameters parameters)
        : Labeled_number_edit{std::move(parameters.title), parameters.initial}
    {}

   public:
    /// Manually set the value of the Number_edit.
    /** Does not emit a value_set Signal. */
    void set_value(Number_t value) { number_edit.set_value(value); }

    /// Return the current value.
    [[nodiscard]] auto value() const noexcept -> Number_t
    {
        return number_edit.value();
    }

   protected:
    auto focus_in_event() -> bool override
    {
        System::set_focus(number_edit);
        return true;
    }
};

/// Helper function to create a Labeled_number_edit instance.
template <typename Number_t = int>
[[nodiscard]] auto labeled_number_edit(Glyph_string title, Number_t initial)
    -> std::unique_ptr<Labeled_number_edit<Number_t>>
{
    return std::make_unique<Labeled_number_edit<Number_t>>(std::move(title),
                                                           initial);
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_NUMBER_EDIT_HPP
