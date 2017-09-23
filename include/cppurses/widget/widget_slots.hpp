#ifndef WIDGET_WIDGET_SLOTS_HPP
#define WIDGET_WIDGET_SLOTS_HPP
#include "painter/color.hpp"
#include "system/key.hpp"
#include <signals/signals.hpp>

namespace cppurses {
class Widget;
namespace slot {

sig::Slot<void()> delete_later(Widget& w);
// sig::Slot<void()> enable(Widget& w);
// sig::Slot<void()> disable(Widget& w);
// sig::Slot<void()> close(Widget& w);
// sig::Slot<void()> hide(Widget& w);
// sig::Slot<void()> show(Widget& w);
// sig::Slot<void()> repaint(Widget& w);  // same as update?
// sig::Slot<void()> update(Widget& w);
// sig::Slot<void(Mouse_button, Coordinates)> click(Widget& w);
// sig::Slot<void(Key)> keypress(Widget& w);
sig::Slot<void(Color)> set_background(Widget& w);
sig::Slot<void(Color)> set_foreground(Widget& w);
// ... any many more.

}  // namespace slot
}  // namespace cppurses

#endif  // WIDGET_WIDGET_SLOTS_HPP
