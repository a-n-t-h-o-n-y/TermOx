#ifndef CPPURSES_WIDGET_WIDGET_SLOTS_HPP
#define CPPURSES_WIDGET_WIDGET_SLOTS_HPP
#include <signals/slot.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/mouse.hpp>

namespace cppurses {
class Widget;
struct Point;
namespace slot {

auto enable(Widget& w) -> sig::Slot<void()>;

auto disable(Widget& w) -> sig::Slot<void()>;

auto update(Widget& w) -> sig::Slot<void()>;

auto click(Widget& w) -> sig::Slot<void(Point, Mouse::Button)>;

auto click(Widget& w, Point p) -> sig::Slot<void(Mouse::Button)>;

auto click(Widget& w, Mouse::Button b) -> sig::Slot<void(Point)>;

auto click(Widget& w, Point p, Mouse::Button b) -> sig::Slot<void()>;

auto keypress(Widget& w) -> sig::Slot<void(Key)>;

auto keypress(Widget& w, Key k) -> sig::Slot<void()>;

auto set_background(Widget& w) -> sig::Slot<void(Color)>;

auto set_background(Widget& w, Color c) -> sig::Slot<void()>;

auto set_foreground(Widget& w) -> sig::Slot<void(Color)>;

auto set_foreground(Widget& w, Color c) -> sig::Slot<void()>;

auto toggle_cursor(Widget& w) -> sig::Slot<void()>;

}  // namespace slot
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGET_SLOTS_HPP
