#ifndef CPPURSES_WIDGET_WIDGETS_NUMBER_EDIT_HPP
#define CPPURSES_WIDGET_WIDGETS_NUMBER_EDIT_HPP
#include <cctype>
#include <sstream>
#include <string>
#include <type_traits>

#include <signals/signal.hpp>

#include <cppurses/widget/widgets/line_edit.hpp>

namespace cppurses {

/// Provide an Line_edit specific to numbers.
template <typename Number_t = int>
class Number_edit : public Line_edit {
   public:
    Number_edit(Number_t initial = 0) : Line_edit{std::to_string(initial)} {
        this->set_validator([this](char c) { return this->is_valid_input(c); });
        this->editing_finished.connect([this](std::string text) {
            auto ss = std::stringstream{text};
            Number_t result = 0;
            ss >> result;
            value_set(result);
        });
    }

    void set_value(Number_t value) {
        std::stringstream ss;
        ss << value;
        this->set_text(ss.str());
    }

    sig::Signal<void(Number_t)> value_set;

   private:
    static bool is_valid_input(char c) {
        auto is_separator = false;
        if (std::is_floating_point<Number_t>::value) {
            is_separator = c == '.' || c == ',';
        }
        return std::isdigit(c) || is_separator;
    }
};
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_NUMBER_EDIT_HPP
