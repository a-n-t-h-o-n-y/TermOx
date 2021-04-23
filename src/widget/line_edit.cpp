#include <termox/widget/widgets/line_edit.hpp>

#include <cctype>
#include <functional>
#include <optional>

#include <signals_light/signal.hpp>

#include <termox/system/key.hpp>
#include <termox/widget/widgets/textbox.hpp>

namespace ox {

auto Line_edit::key_press_event(Key k) -> bool
{
    auto constexpr can_print = [](unsigned char c) {
        return std::isprint(c) || std::isspace(c);
    };

    switch (k) {
        case Key::Enter: {
            enter_pressed.emit(this->Textbox::contents().str());
            if (on_enter_ == Action::Clear)
                this->Textbox::clear();
            return true;
        }
        case Key::Arrow_up:
        case Key::Arrow_down: return true;
        default: break;
    }

    auto const symbol = key_to_char(k);
    if (validator_.has_value() && can_print(symbol) && !((*validator_)(symbol)))
        return true;
    else
        return Textbox::key_press_event(k);
}

}  // namespace ox
