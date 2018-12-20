#include <cppurses/widget/layout.hpp>

#include <memory>
#include <vector>

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/children_data.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widget_free_functions.hpp>

namespace cppurses {

// Free Functions
void set_background(Layout& l, Color c) {
    for (const std::unique_ptr<Widget>& w : l.children.get()) {
        set_background(*w, c);
    }
}

void set_foreground(Layout& l, Color c) {
    for (const std::unique_ptr<Widget>& w : l.children.get()) {
        set_foreground(*w, c);
    }
}

}  // namespace cppurses
