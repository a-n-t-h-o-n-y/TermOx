#include <termox/widget/widgets/log.hpp>

#include <memory>
#include <utility>

#include <termox/painter/glyph_string.hpp>
#include <termox/system/key.hpp>
#include <termox/widget/widgets/text_view.hpp>
#include <termox/widget/widgets/textbox.hpp>

namespace ox {

void Log::post_message(Glyph_string message)
{
    if (!this->text().empty())
        this->append(U"\n");
    this->append(std::move(message));
    this->update_display();
    auto const tl = this->top_line();
    auto const h  = this->area().height;
    auto const lc = this->line_count();
    if (tl + h < lc)
        this->scroll_down(lc - tl - h);
    this->set_cursor(this->text().size());
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

auto log() -> std::unique_ptr<Log> { return std::make_unique<Log>(); }

}  // namespace ox
