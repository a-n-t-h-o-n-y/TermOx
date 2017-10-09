#include "widget/widgets/log.hpp"
#include "system/key.hpp"

#include <signals/slot.hpp>

#include <utility>

namespace cppurses {

void Log::post_message(Glyph_string message) {
    if (!this->contents_empty()) {
        this->append('\n');
    }
    this->append(std::move(message));
    std::size_t tl = this->top_line();
    std::size_t h = this->height();
    std::size_t nol = this->n_of_lines();
    if (tl + h < nol) {
        this->scroll_down(nol - tl - h);
    }
    this->set_cursor(this->contents_size());
}

bool Log::key_press_event(Key key, char symbol) {
    if (key == Key::Arrow_right || key == Key::Arrow_up ||
        key == Key::Arrow_down || key == Key::Arrow_left) {
        return Textbox::key_press_event(key, symbol);
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
