#ifndef WIDGET_WIDGETS_TEXT_DISPLAY_SLOTS_HPP
#define WIDGET_WIDGETS_TEXT_DISPLAY_SLOTS_HPP
#include "widget/widgets/text_display.hpp"
#include "painter/attribute.hpp"
#include "painter/glyph_string.hpp"
#include <signals/slot.hpp>
#include <cstddef>

namespace cppurses {

namespace slot {

// Text Modification
sig::Slot<void()> set_text(Text_display& td, const Glyph_string& text);
sig::Slot<void(Glyph_string)> set_text(Text_display& td);

sig::Slot<void()> insert(Text_display& td,
                         const Glyph_string& text,
                         std::size_t index);
sig::Slot<void(std::size_t)> insert(Text_display& td, const Glyph_string& text);
sig::Slot<void(Glyph_string)> insert(Text_display& td, std::size_t index);
sig::Slot<void(Glyph_string, std::size_t)> insert(Text_display& td);

sig::Slot<void()> append(Text_display& td, const Glyph_string& text);
sig::Slot<void(Glyph_string)> append(Text_display& td);

sig::Slot<void()> erase(Text_display& td,
                        std::size_t index,
                        std::size_t length);
sig::Slot<void(std::size_t)> erase_at(Text_display& td, std::size_t index);
sig::Slot<void(std::size_t)> erase_n(Text_display& td, std::size_t length);
sig::Slot<void(std::size_t, std::size_t)> erase(Text_display& td);

sig::Slot<void()> pop_back(Text_display& td);

sig::Slot<void()> clear(Text_display& td);

// Scrolling
sig::Slot<void()> scroll_up(Text_display& td, std::size_t n);
sig::Slot<void(std::size_t)> scroll_up(Text_display& td);
sig::Slot<void()> scroll_down(Text_display& td, std::size_t n);
sig::Slot<void(std::size_t)> scroll_down(Text_display& td);

// Options
sig::Slot<void()> enable_word_wrap(Text_display& td);
sig::Slot<void()> disable_word_wrap(Text_display& td);
sig::Slot<void()> toggle_word_wrap(Text_display& td);
sig::Slot<void()> set_word_wrap(Text_display& td, bool enable);
sig::Slot<void(bool)> set_word_wrap(Text_display& td);

sig::Slot<void()> add_new_text_attribute(Text_display& td, Attribute attr);
sig::Slot<void(Attribute)> add_new_text_attribute(Text_display& td);
sig::Slot<void()> remove_new_text_attribute(Text_display& td, Attribute attr);
sig::Slot<void(Attribute)> remove_new_text_attribute(Text_display& td);
sig::Slot<void()> clear_new_text_attributes(Text_display& td);

}  // namespace slot

}  // namespace cppurses
#endif  // WIDGET_WIDGETS_TEXT_DISPLAY_SLOTS_HPP
