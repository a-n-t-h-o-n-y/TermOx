#ifndef CPPURSES_WIDGET_WIDGETS_TEXTBOX_BASE_SLOTS_HPP
#define CPPURSES_WIDGET_WIDGETS_TEXTBOX_BASE_SLOTS_HPP
#include <cstddef>

#include <signals/slot.hpp>

#include <cppurses/widget/point.hpp>

namespace cppurses {
class Textbox_base;

namespace slot {

// Cursor Movement
sig::Slot<void()> cursor_up(Textbox_base& tb, std::size_t n);
sig::Slot<void(std::size_t)> cursor_up(Textbox_base& tb);
sig::Slot<void()> cursor_down(Textbox_base& tb, std::size_t n);
sig::Slot<void(std::size_t)> cursor_down(Textbox_base& tb);
sig::Slot<void()> cursor_left(Textbox_base& tb, std::size_t n);
sig::Slot<void(std::size_t)> cursor_left(Textbox_base& tb);
sig::Slot<void()> cursor_right(Textbox_base& tb, std::size_t n);
sig::Slot<void(std::size_t)> cursor_right(Textbox_base& tb);

sig::Slot<void()> set_cursor(Textbox_base& tb, const Point& coords);
sig::Slot<void(Point)> set_cursor(Textbox_base& tb);
sig::Slot<void()> set_cursor_xy(Textbox_base& tb, std::size_t x, std::size_t y);
sig::Slot<void(std::size_t, std::size_t)> set_cursor_xy(Textbox_base& tb);
sig::Slot<void()> set_cursor_to_index(Textbox_base& tb, std::size_t index);
sig::Slot<void(std::size_t)> set_cursor_to_index(Textbox_base& tb);

// Scrolling
sig::Slot<void()> scroll_up(Textbox_base& tb, std::size_t n);
sig::Slot<void(std::size_t)> scroll_up(Textbox_base& tb);
sig::Slot<void()> scroll_down(Textbox_base& tb, std::size_t n);
sig::Slot<void(std::size_t)> scroll_down(Textbox_base& tb);
sig::Slot<void()> enable_scrolling(Textbox_base& tb);
sig::Slot<void()> disable_scrolling(Textbox_base& tb);
sig::Slot<void()> toggle_scrolling(Textbox_base& tb);
sig::Slot<void(bool)> set_scrolling(Textbox_base& tb);

}  // namespace slot
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_TEXTBOX_BASE_SLOTS_HPP
