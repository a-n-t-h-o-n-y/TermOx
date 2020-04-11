#ifndef CPPURSES_WIDGET_WIDGETS_NUMBER_EDIT_HPP
#define CPPURSES_WIDGET_WIDGETS_NUMBER_EDIT_HPP
#include <cctype>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>

#include <signals/signal.hpp>

#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/widgets/line_edit.hpp>

namespace cppurses {

/// A Line_edit for specified number types.
/** Provides validator specific to Number_t. Uses std::stringstream to convert
 *  from string to Number_t value. */
template <typename Number_t = int>
class Number_edit : public Line_edit {
   public:
    /// Emitted on Enter Key press, sends along the current value.
    sig::Signal<void(Number_t)> value_set;

   public:
    /// Construct a Number_edit with \p initial value.
    Number_edit(Number_t initial = 0)
        : Line_edit{std::to_string(initial)}, value_{initial}
    {
        // this->set_alignment(Align::Right); // TODO Once alignment works
        this->set_validator(
            [](char c) { return Number_edit::is_valid_input(c); });
        this->edit_finished.connect([this](std::string text) {
            auto ss     = std::stringstream{text};
            auto result = Number_t{0};
            ss >> result;
            value_set(result);
        });
        this->set_value(initial);
    }

    /// Manually set the value of the Number_edit.
    /** Does not emit a value_set Signal. */
    void set_value(Number_t value)
    {
        value_ = value;
        this->display_value();
    }

   protected:
    auto mouse_wheel_event(Mouse::State const& mouse) -> bool override
    {
        switch (mouse.button) {
            case Mouse::Button::ScrollUp: this->increment(); break;
            case Mouse::Button::ScrollDown: this->decrement(); break;
            default: break;
        }
        return Line_edit::mouse_wheel_event(mouse);
    }

   private:
    Number_t value_;

   private:
    /// Verifies if \p c is a valid input for Number_t.
    static auto is_valid_input(char c) -> bool
    {
        auto is_separator = false;
        if (std::is_floating_point<Number_t>::value)
            is_separator = (c == '.' or c == ',');
        auto is_sign = (c == '+');
        if (not std::is_unsigned<Number_t>::value)
            is_sign |= (c == '-');
        return std::isdigit(c) or is_separator or is_sign;
    }

    void display_value()
    {
        auto ss = std::stringstream{};
        ss << value_;
        this->set_contents(ss.str());
    }

    void emit_signal() { value_set(value_); }

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

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_NUMBER_EDIT_HPP
