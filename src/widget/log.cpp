#include <cppurses/widget/widgets/log.hpp>

#include <utility>

#include <signals/slot.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/key.hpp>

namespace cppurses {

void Log::post_message(Glyph_string message)
{
    if (!this->contents().empty())
        this->append(L"\n");
    this->append(std::move(message));
    this->update_display();
    auto const tl = this->top_line();
    auto const h  = this->height();
    auto const lc = this->line_count();
    if (tl + h < lc)
        this->scroll_down(lc - tl - h);
    this->set_cursor(this->contents().size());
}

auto Log::key_press_event(Key k) -> bool
{
    switch (k) {
        case Key::Arrow_right:
        case Key::Arrow_left:
        case Key::Arrow_up:
        case Key::Arrow_down: return Textbox::key_press_event(k);
        default: return true;
    }
}

namespace slot {

auto post_message(Log& log) -> sig::Slot<void(Glyph_string)>
{
    auto slot = sig::Slot<void(Glyph_string)>{
        [&log](Glyph_string message) { log.post_message(std::move(message)); }};
    slot.track(log.destroyed);
    return slot;
}

auto post_message(Log& log, Glyph_string const& message) -> sig::Slot<void()>
{
    auto slot =
        sig::Slot<void()>{[&log, message] { log.post_message(message); }};
    slot.track(log.destroyed);
    return slot;
}

}  // namespace slot
}  // namespace cppurses
