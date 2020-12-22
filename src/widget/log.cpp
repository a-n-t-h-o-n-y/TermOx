#include <termox/widget/widgets/log.hpp>

#include <utility>

#include <signals_light/signal.hpp>

#include <termox/painter/glyph_string.hpp>
#include <termox/system/key.hpp>
#include <termox/widget/detail/link_lifetimes.hpp>

namespace ox {

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

}  // namespace ox

namespace ox::slot {

auto post_message(Log& log) -> sl::Slot<void(Glyph_string)>
{
    return link_lifetimes(
        [&log](Glyph_string message) { log.post_message(std::move(message)); },
        log);
}

auto post_message(Log& log, Glyph_string const& message) -> sl::Slot<void()>
{
    return link_lifetimes([&log, message] { log.post_message(message); }, log);
}

}  // namespace ox::slot
