#include <cppurses/widget/widgets/line_edit.hpp>

#include <cctype>
#include <functional>
#include <utility>
#include <vector>

#include <signals/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/trait.hpp>
#include <cppurses/system/events/key.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widgets/textbox.hpp>

namespace cppurses {

void Line_edit::underline(bool enabled)
{
    if (enabled) {
        this->set_wallpaper({L' ', Trait::Underline});
        auto underlined_text = this->contents();
        underlined_text.add_traits(Trait::Underline);
        this->set_contents(std::move(underlined_text));
        this->insert_brush.add_traits(Trait::Underline);
    }
    else {
        this->set_wallpaper(L' ');
        auto non_underlined = this->contents();
        non_underlined.remove_traits(Trait::Underline);
        this->set_contents(std::move(non_underlined));
        this->insert_brush.remove_traits(Trait::Underline);
    }
    this->update();
}

void Line_edit::set_ghost_color(Color c)
{
    if (not on_initial_)
        return;
    auto ghost_text = this->contents();
    ghost_text.add_traits(foreground(c));
    this->set_contents(std::move(ghost_text));
    this->update();
}

auto Line_edit::key_press_event(Key::State const& keyboard) -> bool
{
    switch (keyboard.key) {
        case Key::Enter:
            edit_finished(this->contents().str());
            if (clear_on_enter_)
                this->clear();
            if (on_initial_)
                on_initial_ = false;
            return true;
        case Key::Arrow_up:
        case Key::Arrow_down: return true;
        default: break;
    }
    auto const is_printable = [](char c) {
        return std::isprint(c) or std::isspace(c);
    };
    if (not is_printable(keyboard.symbol))
        return Textbox::key_press_event(keyboard);
    if (not validator_(keyboard.symbol))
        return true;
    if (keyboard.symbol != '\0' and on_initial_) {  // First alpha-num input
        this->clear();
        on_initial_ = false;
    }
    return Textbox::key_press_event(keyboard);
}

}  // namespace cppurses
