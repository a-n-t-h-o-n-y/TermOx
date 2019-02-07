#ifndef CPPURSES_WIDGET_WIDGETS_NUMBER_EDIT_HPP
#define CPPURSES_WIDGET_WIDGETS_NUMBER_EDIT_HPP
#include <cctype>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

#include <signals/signal.hpp>

#include <cppurses/widget/layouts/horizontal_layout.hpp>
#include <cppurses/widget/widgets/label.hpp>
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
        // this->set_alignment(Alignment::Right); // TODO for once align works
    }

    void set_value(Number_t value) {
        std::stringstream ss;
        ss << value;
        this->set_contents(ss.str());
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

template <typename Number_t = int>
struct Labeled_number_edit : cppurses::Horizontal_layout {
    Labeled_number_edit(Glyph_string title, Number_t initial)
        : label{this->make_child<cppurses::Label>(std::move(title))},
          number_edit{
              this->make_child<cppurses::Number_edit<Number_t>>(initial)} {
        this->height_policy.type(cppurses::Size_policy::Fixed);
        this->height_policy.hint(1);
        label.width_policy.type(cppurses::Size_policy::Fixed);
        label.width_policy.hint(label.contents().size());

        number_edit.brush.set_background(cppurses::Color::White);
        number_edit.brush.set_foreground(cppurses::Color::Black);
        number_edit.set_ghost_color(cppurses::Color::Black);
    }

    cppurses::Label& label;
    cppurses::Number_edit<Number_t>& number_edit;

    sig::Signal<void(Number_t)>& value_set{number_edit.value_set};
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_NUMBER_EDIT_HPP
