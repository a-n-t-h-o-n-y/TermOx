#ifndef CPPURSES_WIDGET_WIDGET_SLOTS_HPP
#define CPPURSES_WIDGET_WIDGET_SLOTS_HPP
#include <signals/slot.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/system/events/key.hpp>
#include <cppurses/system/events/mouse.hpp>

namespace cppurses {
class Widget;
struct Point;
namespace slot {

sig::Slot<void()> enable(Widget& w);

sig::Slot<void()> disable(Widget& w);

sig::Slot<void()> delete_later(Widget& w);

sig::Slot<void()> update(Widget& w);

sig::Slot<void(Point, Mouse::Button)> click(Widget& w);

sig::Slot<void(Mouse::Button)> click(Widget& w, Point c);

sig::Slot<void(Point)> click(Widget& w, Mouse::Button b);

sig::Slot<void()> click(Widget& w, Point c, Mouse::Button b);

sig::Slot<void(Key::Code)> keypress(Widget& w);

sig::Slot<void()> keypress(Widget& w, Key::Code k);

sig::Slot<void(Color)> set_background(Widget& w);

sig::Slot<void()> set_background(Widget& w, Color c);

sig::Slot<void(Color)> set_foreground(Widget& w);

sig::Slot<void()> set_foreground(Widget& w, Color c);

sig::Slot<void()> toggle_cursor(Widget& w);

}  // namespace slot
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGET_SLOTS_HPP
