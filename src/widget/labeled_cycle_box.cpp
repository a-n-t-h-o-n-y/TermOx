#include <cppurses/widget/widgets/labeled_cycle_box.hpp>

#include <signals/signals.hpp>

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

namespace cppurses {

Labeled_cycle_box::Labeled_cycle_box(Glyph_string title) {
    // this->height_policy.type(Size_policy::Fixed);
    // this->height_policy.hint(1);

    this->set_title(std::move(title));

    disable_walls(label.border);
    disable_corners(label.border);
    label.border.east_enabled = true;
    label.border.east = "├";
    enable_border(label);
}

void Labeled_cycle_box::set_title(Glyph_string title) {
    label.set_text(std::move(title));
    this->resize_label();
}

void Labeled_cycle_box::resize_label() {
    label.width_policy.hint(label.contents().size() + 2);
    label.width_policy.type(Size_policy::Fixed);
}

}  // namespace cppurses
