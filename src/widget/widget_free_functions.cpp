#include <cppurses/widget/widget_free_functions.hpp>

#include <cstddef>

#include <cppurses/painter/color.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

bool has_border(const Widget& w) {
    return w.border.enabled;
}

void enable_border(Widget& w) {
    w.border.enabled = true;
}

void disable_border(Widget& w) {
    w.border.enabled = false;
}

bool has_coordinates(const Widget& w,
                     std::size_t global_x,
                     std::size_t global_y) {
    if (!w.enabled()) {
        return false;
    }
    const bool within_west = global_x >= w.inner_x();
    const bool within_east = global_x < (w.inner_x() + w.width());
    const bool within_north = global_y >= w.inner_y();
    const bool within_south = global_y < (w.inner_y() + w.height());
    return within_west && within_east && within_north && within_south;
}

void set_background(Widget& w, Color c) {
    w.brush.set_background(c);
    w.update();
    w.background_color_changed(c);
}

void set_foreground(Widget& w, Color c) {
    w.brush.set_foreground(c);
    w.update();
    w.foreground_color_changed(c);
}

void clear_attributes(Widget& w) {
    w.brush.clear_attributes();
    w.update();
}

void set_background_recursive(Widget& w, Color c, bool single_level) {
    set_background(w, c);
    for (const std::unique_ptr<Widget>& child : w.children.get()) {
        if (single_level) {
            set_background(*child, c);
        } else {
            set_background_recursive(*child, c, single_level);
        }
    }
}

void set_foreground_recursive(Widget& w, Color c, bool single_level) {
    set_foreground(w, c);
    for (const std::unique_ptr<Widget>& child : w.children.get()) {
        if (single_level) {
            set_foreground(*child, c);
        } else {
            set_foreground_recursive(*child, c, single_level);
        }
    }
}

bool has_focus(const Widget& w) {
    return Focus::focus_widget() == &w;
}

}  // namespace cppurses
