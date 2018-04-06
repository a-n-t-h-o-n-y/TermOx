#include <cppurses/widget/widgets/text_display_slots.hpp>

#include <cstddef>
#include <utility>

#include <signals/slot.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

namespace cppurses {

namespace slot {

// Text Modification
sig::Slot<void()> set_text(Text_display& td, const Glyph_string& text) {
    sig::Slot<void()> slot{[&td, text] { td.set_text(text); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void(Glyph_string)> set_text(Text_display& td) {
    sig::Slot<void(Glyph_string)> slot{
        [&td](auto text) { td.set_text(std::move(text)); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void()> insert(Text_display& td,
                         const Glyph_string& text,
                         std::size_t index) {
    sig::Slot<void()> slot{[&td, text, index] { td.insert(text, index); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> insert(Text_display& td,
                                    const Glyph_string& text) {
    sig::Slot<void(std::size_t)> slot{
        [&td, text](auto index) { td.insert(text, index); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void(Glyph_string)> insert(Text_display& td, std::size_t index) {
    sig::Slot<void(Glyph_string)> slot{
        [&td, index](auto text) { td.insert(std::move(text), index); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void(Glyph_string, std::size_t)> insert(Text_display& td) {
    sig::Slot<void(Glyph_string, std::size_t)> slot{
        [&td](auto text, auto index) {
            td.insert(std::move(text), std::move(index));
        }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void()> append(Text_display& td, const Glyph_string& text) {
    sig::Slot<void()> slot{[&td, text] { td.append(text); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void(Glyph_string)> append(Text_display& td) {
    sig::Slot<void(Glyph_string)> slot{
        [&td](auto text) { td.append(std::move(text)); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void()> erase(Text_display& td,
                        std::size_t index,
                        std::size_t length) {
    sig::Slot<void()> slot{[&td, index, length] { td.erase(index, length); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> erase_at(Text_display& td, std::size_t index) {
    sig::Slot<void(std::size_t)> slot{
        [&td, index](auto length) { td.erase(index, std::move(length)); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> erase_n(Text_display& td, std::size_t length) {
    sig::Slot<void(std::size_t)> slot{
        [&td, length](auto index) { td.erase(std::move(index), length); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void(std::size_t, std::size_t)> erase(Text_display& td) {
    sig::Slot<void(std::size_t, std::size_t)> slot{
        [&td](auto index, auto length) {
            td.erase(std::move(index), std::move(length));
        }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void()> pop_back(Text_display& td) {
    sig::Slot<void()> slot{[&td] { td.pop_back(); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void()> clear(Text_display& td) {
    sig::Slot<void()> slot{[&td] { td.clear(); }};
    slot.track(td.destroyed);
    return slot;
}

// Scrolling
sig::Slot<void()> scroll_up(Text_display& td, std::size_t n) {
    sig::Slot<void()> slot{[&td, n] { td.scroll_up(n); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> scroll_up(Text_display& td) {
    sig::Slot<void(std::size_t)> slot{[&td](auto n) { td.scroll_up(n); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void()> scroll_down(Text_display& td, std::size_t n) {
    sig::Slot<void()> slot{[&td, n] { td.scroll_down(n); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> scroll_down(Text_display& td) {
    sig::Slot<void(std::size_t)> slot{[&td](auto n) { td.scroll_down(n); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void()> enable_word_wrap(Text_display& td) {
    sig::Slot<void()> slot{[&td] { td.enable_word_wrap(); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void()> disable_word_wrap(Text_display& td) {
    sig::Slot<void()> slot{[&td] { td.disable_word_wrap(); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void()> toggle_word_wrap(Text_display& td) {
    sig::Slot<void()> slot{[&td] { td.toggle_word_wrap(); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void()> set_word_wrap(Text_display& td, bool enable) {
    sig::Slot<void()> slot{[&td, enable] { td.enable_word_wrap(enable); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void(bool)> set_word_wrap(Text_display& td) {
    sig::Slot<void(bool)> slot{
        [&td](bool enable) { td.enable_word_wrap(enable); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void()> add_new_text_attribute(Text_display& td, Attribute attr) {
    sig::Slot<void()> slot{[&td, attr] { td.add_new_text_attribute(attr); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void(Attribute)> add_new_text_attribute(Text_display& td) {
    sig::Slot<void(Attribute)> slot{
        [&td](auto attr) { td.add_new_text_attribute(attr); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void()> remove_new_text_attribute(Text_display& td, Attribute attr) {
    sig::Slot<void()> slot{[&td, attr] { td.remove_new_text_attribute(attr); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void(Attribute)> remove_new_text_attribute(Text_display& td) {
    sig::Slot<void(Attribute)> slot{
        [&td](auto attr) { td.remove_new_text_attribute(attr); }};
    slot.track(td.destroyed);
    return slot;
}

sig::Slot<void()> clear_new_text_attributes(Text_display& td) {
    sig::Slot<void()> slot{[&td] { td.clear_new_text_attributes(); }};
    slot.track(td.destroyed);
    return slot;
}

}  // namespace slot

}  // namespace cppurses
