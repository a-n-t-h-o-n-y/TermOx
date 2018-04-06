#ifndef WIDGET_WIDGETS_TEXTBOX_SLOTS_HPP
#define WIDGET_WIDGETS_TEXTBOX_SLOTS_HPP
#include <cstddef>

#include <signals/slot.hpp>

namespace cppurses {
class Textbox;

namespace slot {

sig::Slot<void()> enable_scrollwheel(Textbox& tb);
sig::Slot<void()> disable_scrollwheel(Textbox& tb);
sig::Slot<void()> toggle_scrollwheel(Textbox& tb);
sig::Slot<void()> set_scrollwheel(Textbox& tb, bool enable);
sig::Slot<void(bool)> set_scrollwheel(Textbox& tb);

sig::Slot<void()> set_wheel_speed(Textbox& tb, std::size_t lines);
sig::Slot<void(std::size_t)> set_wheel_speed(Textbox& tb);

sig::Slot<void()> set_wheel_speed_up(Textbox& tb, std::size_t lines);
sig::Slot<void(std::size_t)> set_wheel_speed_up(Textbox& tb);

sig::Slot<void()> set_wheel_speed_down(Textbox& tb, std::size_t lines);
sig::Slot<void(std::size_t)> set_wheel_speed_down(Textbox& tb);

}  // namespace slot

}  // namespace cppurses

#endif  // WIDGET_WIDGETS_TEXTBOX_SLOTS_HPP
