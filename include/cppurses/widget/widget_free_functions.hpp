#ifndef CPPURSES_WIDGET_WIDGET_FREE_FUNCTIONS_HPP
#define CPPURSES_WIDGET_WIDGET_FREE_FUNCTIONS_HPP
#include <cstddef>

#include <cppurses/painter/color.hpp>

namespace cppurses {
class Widget;

bool has_border(const Widget& w);

void enable_border(Widget& w);

void disable_border(Widget& w);

bool has_coordinates(Widget& w, std::size_t global_x, std::size_t global_y);

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

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGET_FREE_FUNCTIONS_HPP
