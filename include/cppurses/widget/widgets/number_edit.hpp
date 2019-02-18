#ifndef CPPURSES_WIDGET_WIDGETS_NUMBER_EDIT_HPP
#define CPPURSES_WIDGET_WIDGETS_NUMBER_EDIT_HPP
#include <cctype>
#include <sstream>
#include <string>
#include <type_traits>

#include <signals/signal.hpp>

#include <cppurses/widget/widgets/line_edit.hpp>

namespace cppurses {

// TODO add increment via scrolling, and possibly arrows on either side, though
// that adds 2 extra cells that can't be used for numbers, can you make it an
// option?

/// A Line_edit for specified number types.
/** Provides validator specific to Number_t. Uses std::stringstream to convert
 *  from string to Number_t value. */
template <typename Number_t = int>
class Number_edit : public Line_edit {
   public:
    /// Construct a Number_edit with \p initial value.
    Number_edit(Number_t initial = 0);

    /// Manually set the value of the Number_edit.
    /** Does not emit a value_set Signal. */
    void set_value(Number_t value);

    /// Emitted on Enter Key press, sends along the current value.
    sig::Signal<void(Number_t)> value_set;

   private:
    /// Verifies if \p c is a valid input for Number_t.
    static bool is_valid_input(char c);
};

template <typename Number_t>
Number_edit<Number_t>::Number_edit(Number_t initial)
    : Line_edit{std::to_string(initial)} {
    this->set_name("Number_edit");
    // this->set_alignment(Alignment::Right); // TODO for once align works
    this->set_validator([](char c) { return Number_edit::is_valid_input(c); });
    this->edit_finished.connect([this](std::string text) {
        auto ss = std::stringstream{text};
        Number_t result = 0;
        ss >> result;
        value_set(result);
    });
}

template <typename Number_t>
void Number_edit<Number_t>::set_value(Number_t value) {
    std::stringstream ss;
    ss << value;
    this->set_contents(ss.str());
}

template <typename Number_t>
bool Number_edit<Number_t>::is_valid_input(char c) {
    auto is_separator = false;
    if (std::is_floating_point<Number_t>::value) {
        is_separator = c == '.' || c == ',';
    }
    auto is_sign = c == '+';
    if (!std::is_unsigned<Number_t>::value) {
        is_sign = is_sign || c == '-';
    }
    return std::isdigit(c) || is_separator || is_sign;
}
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_NUMBER_EDIT_HPP
