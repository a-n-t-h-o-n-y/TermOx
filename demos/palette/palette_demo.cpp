#include "palette_demo.hpp"

#include <cppurses/widget/size_policy.hpp>

namespace palette {

Palette_demo::Palette_demo() {
    all_colors_display_.color_select.color_changed.connect(
        [this](cppurses::Color c) { color_setter_.change_current_color(c); });
}

}  // namespace palette
