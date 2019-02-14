#include <cppurses/widget/widgets/labeled_cycle_box.hpp>

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include <signals/signals.hpp>

#include <cppurses/widget/widget_free_functions.hpp>

namespace cppurses {

Labeled_cycle_box::Labeled_cycle_box(Glyph_string title) {
    this->set_title(std::move(title));

    label.border.enable();
    label.border.segments.disable_all();
    label.border.segments.east.enable();
    label.border.segments.east = L'├';
}

void Labeled_cycle_box::set_title(Glyph_string title) {
    label.set_contents(std::move(title));
    this->resize_label();
}

void Labeled_cycle_box::resize_label() {
    label.width_policy.hint(label.contents().size() + 2);
    label.width_policy.type(Size_policy::Fixed);
    this->update();
}

void Labeled_cycle_box::set_divider(const Glyph& divider) {
    label.border.segments.east = divider;
    this->update();
}

}  // namespace cppurses
