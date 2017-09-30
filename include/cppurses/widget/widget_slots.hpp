#ifndef WIDGET_WIDGET_SLOTS_HPP
#define WIDGET_WIDGET_SLOTS_HPP
#include "painter/color.hpp"
#include "system/key.hpp"
#include "widget/coordinates.hpp"
#include "system/events/mouse_event.hpp"
#include <signals/slot.hpp>

namespace cppurses {
class Widget;
namespace slot {

sig::Slot<void()> delete_later(Widget& w);
sig::Slot<void()> close(Widget& w);
sig::Slot<void()> hide(Widget& w);
sig::Slot<void()> show(Widget& w);
sig::Slot<void()> repaint(Widget& w);
sig::Slot<void()> update(Widget& w);
sig::Slot<void(Coordinates, Mouse_button)> click(Widget& w);
sig::Slot<void(Mouse_button)> click(Widget& w, Coordinates c);
sig::Slot<void(Coordinates)> click(Widget& w, Mouse_button b);
sig::Slot<void()> click(Widget& w, Coordinates c, Mouse_button b);
sig::Slot<void(Key)> keypress(Widget& w);
sig::Slot<void()> keypress(Widget& w, Key k);
sig::Slot<void(Color)> set_background(Widget& w);
sig::Slot<void()> set_background(Widget& w, Color c);
sig::Slot<void(Color)> set_foreground(Widget& w);
sig::Slot<void()> set_foreground(Widget& w, Color c);

}  // namespace slot
}  // namespace cppurses

#endif  // WIDGET_WIDGET_SLOTS_HPP
