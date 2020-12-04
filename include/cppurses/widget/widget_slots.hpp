#ifndef CPPURSES_WIDGET_WIDGET_SLOTS_HPP
#define CPPURSES_WIDGET_WIDGET_SLOTS_HPP
#include <signals_light/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/mouse.hpp>

namespace cppurses {
class Widget;
struct Point;
}  // namespace cppurses

namespace cppurses::slot {

auto enable(Widget& w) -> sl::Slot<void()>;

auto disable(Widget& w) -> sl::Slot<void()>;

auto update(Widget& w) -> sl::Slot<void()>;

auto click(Widget& w) -> sl::Slot<void(Point, Mouse::Button)>;

auto click(Widget& w, Point p) -> sl::Slot<void(Mouse::Button)>;

auto click(Widget& w, Mouse::Button b) -> sl::Slot<void(Point)>;

auto click(Widget& w, Point p, Mouse::Button b) -> sl::Slot<void()>;

auto keypress(Widget& w) -> sl::Slot<void(Key)>;

auto keypress(Widget& w, Key k) -> sl::Slot<void()>;

auto set_background(Widget& w) -> sl::Slot<void(Color)>;

auto set_background(Widget& w, Color c) -> sl::Slot<void()>;

auto set_foreground(Widget& w) -> sl::Slot<void(Color)>;

auto set_foreground(Widget& w, Color c) -> sl::Slot<void()>;

auto toggle_cursor(Widget& w) -> sl::Slot<void()>;

}  // namespace cppurses::slot
#endif  // CPPURSES_WIDGET_WIDGET_SLOTS_HPP
