#include <cppurses/widget/coordinates.hpp>
#include <cppurses/widget/widgets/textbox_base.hpp>
#include <cppurses/widget/widgets/textbox_base_slots.hpp>

#include <signals/slot.hpp>

#include <cstddef>

namespace cppurses {

namespace slot {

sig::Slot<void()> cursor_up(Textbox_base& tb, std::size_t n) {
    sig::Slot<void()> slot{[&tb, n] { tb.cursor_up(n); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> cursor_up(Textbox_base& tb) {
    sig::Slot<void(std::size_t)> slot{[&tb](auto n) { tb.cursor_up(n); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> cursor_down(Textbox_base& tb, std::size_t n) {
    sig::Slot<void()> slot{[&tb, n] { tb.cursor_down(n); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> cursor_down(Textbox_base& tb) {
    sig::Slot<void(std::size_t)> slot{[&tb](auto n) { tb.cursor_down(n); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> cursor_left(Textbox_base& tb, std::size_t n) {
    sig::Slot<void()> slot{[&tb, n] { tb.cursor_left(n); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> cursor_left(Textbox_base& tb) {
    sig::Slot<void(std::size_t)> slot{[&tb](auto n) { tb.cursor_up(n); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> cursor_right(Textbox_base& tb, std::size_t n) {
    sig::Slot<void()> slot{[&tb, n] { tb.cursor_right(n); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> cursor_right(Textbox_base& tb) {
    sig::Slot<void(std::size_t)> slot{[&tb](auto n) { tb.cursor_right(n); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> set_cursor(Textbox_base& tb, const Coordinates& coords) {
    sig::Slot<void()> slot{[&tb, coords] { tb.set_cursor(coords); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void(Coordinates)> set_cursor(Textbox_base& tb) {
    sig::Slot<void(Coordinates)> slot{
        [&tb](auto coords) { tb.set_cursor(coords); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> set_cursor_xy(Textbox_base& tb,
                                std::size_t x,
                                std::size_t y) {
    sig::Slot<void()> slot{[&tb, x, y] { tb.set_cursor(x, y); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void(std::size_t, std::size_t)> set_cursor_xy(Textbox_base& tb) {
    sig::Slot<void(std::size_t, std::size_t)> slot{
        [&tb](auto x, auto y) { tb.set_cursor(x, y); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> set_cursor_to_index(Textbox_base& tb, std::size_t index) {
    sig::Slot<void()> slot{[&tb, index] { tb.set_cursor(index); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> set_cursor_to_index(Textbox_base& tb) {
    sig::Slot<void(std::size_t)> slot{
        [&tb](auto index) { tb.set_cursor(index); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> scroll_up(Textbox_base& tb, std::size_t n) {
    sig::Slot<void()> slot{[&tb, n] { tb.scroll_up(n); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> scroll_up(Textbox_base& tb) {
    sig::Slot<void(std::size_t)> slot{[&tb](auto n) { tb.scroll_up(n); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> scroll_down(Textbox_base& tb, std::size_t n) {
    sig::Slot<void()> slot{[&tb, n] { tb.scroll_down(n); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> scroll_down(Textbox_base& tb) {
    sig::Slot<void(std::size_t)> slot{[&tb](auto n) { tb.scroll_down(n); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> enable_scrolling(Textbox_base& tb) {
    sig::Slot<void()> slot{[&tb] { tb.enable_scrolling(); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> disable_scrolling(Textbox_base& tb) {
    sig::Slot<void()> slot{[&tb] { tb.disable_scrolling(); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void()> toggle_scrolling(Textbox_base& tb) {
    sig::Slot<void()> slot{[&tb] { tb.toggle_scrolling(); }};
    slot.track(tb.destroyed);
    return slot;
}

sig::Slot<void(bool)> set_scrolling(Textbox_base& tb) {
    sig::Slot<void(bool)> slot{
        [&tb](bool enable) { tb.enable_scrolling(enable); }};
    slot.track(tb.destroyed);
    return slot;
}

}  // namespace slot

}  // namespace cppurses
