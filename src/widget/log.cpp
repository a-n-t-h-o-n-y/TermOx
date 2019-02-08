#include <cppurses/widget/widgets/log.hpp>

#include <cstddef>
#include <utility>

#include <signals/slot.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/keyboard_data.hpp>

namespace cppurses {

void Log::post_message(Glyph_string message) {
    if (!this->contents().empty()) {
        this->append('\n');
    }
    this->append(std::move(message));
    this->update_display();
    std::size_t tl = this->top_line();
    std::size_t h = this->height();
    std::size_t nol = this->line_count();
    if (tl + h < nol) {
        this->scroll_down(nol - tl - h);
    }
    this->set_cursor(this->contents().size());
}

bool Log::key_press_event(const Keyboard_data& keyboard) {
    if (keyboard.key == Key::Arrow_right || keyboard.key == Key::Arrow_up ||
        keyboard.key == Key::Arrow_down || keyboard.key == Key::Arrow_left) {
        return Textbox::key_press_event(keyboard);
    }
    return true;
}

namespace slot {

sig::Slot<void(Glyph_string)> post_message(Log& log) {
    sig::Slot<void(Glyph_string)> slot{
        [&log](Glyph_string message) { log.post_message(std::move(message)); }};
    slot.track(log.destroyed);
    return slot;
}

sig::Slot<void()> post_message(Log& log, const Glyph_string& message) {
    sig::Slot<void()> slot{[&log, message] { log.post_message(message); }};
    slot.track(log.destroyed);
    return slot;
}

}  // namespace slot
}  // namespace cppurses
