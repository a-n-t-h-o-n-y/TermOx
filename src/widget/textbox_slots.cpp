#include <cppurses/widget/widgets/textbox_slots.hpp>

#include <cstddef>

#include <signals/slot.hpp>

#include <cppurses/widget/widgets/textbox.hpp>

namespace cppurses {

namespace slot {

sig::Slot<void()> enable_scrollwheel(Textbox& tb) {
    sig::Slot<void()> slot{[&tb] { tb.enable_scrollwheel(); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> disable_scrollwheel(Textbox& tb) {
    sig::Slot<void()> slot{[&tb] { tb.disable_scrollwheel(); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> toggle_scrollwheel(Textbox& tb) {
    sig::Slot<void()> slot{[&tb] { tb.toggle_scrollwheel(); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> set_scrollwheel(Textbox& tb, bool enable) {
    sig::Slot<void()> slot{[&tb, enable] { tb.enable_scrollwheel(enable); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void(bool)> set_scrollwheel(Textbox& tb) {
    sig::Slot<void(bool)> slot{
        [&tb](bool enable) { tb.enable_scrollwheel(enable); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> set_wheel_speed(Textbox& tb, std::size_t lines) {
    sig::Slot<void()> slot{[&tb, lines] { tb.set_wheel_speed(lines); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> set_wheel_speed(Textbox& tb) {
    sig::Slot<void(std::size_t)> slot{
        [&tb](auto lines) { tb.set_wheel_speed(lines); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> set_wheel_speed_up(Textbox& tb, std::size_t lines) {
    sig::Slot<void()> slot{[&tb, lines] { tb.set_wheel_speed_up(lines); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> set_wheel_speed_up(Textbox& tb) {
    sig::Slot<void(std::size_t)> slot{
        [&tb](auto lines) { tb.set_wheel_speed_up(lines); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> set_wheel_speed_down(Textbox& tb, std::size_t lines) {
    sig::Slot<void()> slot{[&tb, lines] { tb.set_wheel_speed_down(lines); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> set_wheel_speed_down(Textbox& tb) {
    sig::Slot<void(std::size_t)> slot{
        [&tb](auto lines) { tb.set_wheel_speed_down(lines); }};
    slot.track(tb.destroyed);
    return slot;
}

}  // namespace slot

}  // namespace cppurses
