#ifndef CPPURSES_WIDGET_WIDGETS_LABELED_NUMBER_EDIT_HPP
#define CPPURSES_WIDGET_WIDGETS_LABELED_NUMBER_EDIT_HPP
#include <utility>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/number_edit.hpp>

namespace cppurses {

/// Number_edit with preceding Label arranged horizontally.
template <typename Number_t = int>
struct Labeled_number_edit : cppurses::layout::Horizontal {
    cppurses::Label& label;
    cppurses::Number_edit<Number_t>& number_edit;

    /// Construct with \p title for Label text and \p initial value.
    Labeled_number_edit(Glyph_string title, Number_t initial);

    /// Manually set the value of the Number_edit.
    /** Does not emit a value_set Signal. Forwards to number_edit object.*/
    void set_value(Number_t value) { number_edit.set_value(value); }

    /// Emitted on Enter Key press, sends along the current value.
    sig::Signal<void(Number_t)>& value_set{number_edit.value_set};
};

template <typename Number_t>
Labeled_number_edit<Number_t>::Labeled_number_edit(Glyph_string title,
                                                   Number_t initial)
    : label{this->make_child<cppurses::Label>(std::move(title))},
      number_edit{this->make_child<cppurses::Number_edit<Number_t>>(initial)} {
    this->height_policy.fixed(1);
    label.width_policy.fixed(label.contents().size());
    number_edit.brush.set_background(cppurses::Color::White);
    number_edit.brush.set_foreground(cppurses::Color::Black);
    number_edit.set_ghost_color(cppurses::Color::Gray);
}
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_LABELED_NUMBER_EDIT_HPP
