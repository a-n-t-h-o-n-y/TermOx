#ifndef CPPURSES_WIDGET_WIDGET_FREE_FUNCTIONS_HPP
#define CPPURSES_WIDGET_WIDGET_FREE_FUNCTIONS_HPP
#include <cstddef>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
class Widget;

bool has_coordinates(const Widget& w, std::size_t global_x, std::size_t global_y);

// TODO You should no longer need a repaint event for these functions, if the
// background changes in a noticable way, it will be repainted by flush()
void set_background(Widget& w, Color c);

void set_foreground(Widget& w, Color c);

template <typename... Attrs>
void add_attributes(Widget& w, Attrs&... attrs);

template <typename... Attrs>
void remove_attributes(Widget& w, Attrs&... attrs);

void clear_attributes(Widget& w);

void set_background_recursive(Widget& w, Color c, bool single_level = false);

void set_foreground_recursive(Widget& w, Color c, bool single_level = false);

bool has_focus(const Widget& w);

template <typename... Attrs>
void add_attributes(Widget& w, Attrs&&... attrs) {
    for (Attribute a : {attrs...}) {
        if (!w.brush.has_attribute(a)) {
            w.brush.add_attributes(std::forward<Attrs>(attrs)...);
            w.update();
            return;
        }
    }
}

template <typename... Attrs>
void remove_attributes(Widget& w, Attrs&&... attrs) {
    bool repaint{false};
    for (const auto& a : {attrs...}) {
        if (w.brush.has_attribute(a)) {
            w.brush.remove_attribute(a);
            repaint = true;
        }
    }
    if (repaint) {
        w.update();
    }
}

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGET_FREE_FUNCTIONS_HPP
