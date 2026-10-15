#include <ox/command.hpp>
#include <ox/selection.hpp>
namespace ox {
auto key_label(KeyChord const& chord, bool ascii) -> std::string
{
    auto text = std::string{};
    if (chord.modifiers.ctrl) text += "ctrl+";
    if (chord.modifiers.alt) text += "alt+";
    if (chord.modifiers.shift) text += "shift+";
    if (chord.modifiers.meta) text += "meta+";
    if (chord.modifiers.super) text += "super+";
    if (chord.modifiers.hyper) text += "hyper+";
    std::visit(
        [&](auto key) {
            using T = decltype(key);
            if constexpr (std::same_as<T, char32_t>) {
                if (key == U' ')
                    text += "space";
                else
                    text += esc::utf32_to_utf8(std::u32string_view{&key, 1});
            }
            else if constexpr (std::same_as<T, KeyCode>) {
                switch (key) {
                    case KeyCode::ArrowUp: text += ascii ? "up" : "↑"; break;
                    case KeyCode::ArrowDown: text += ascii ? "down" : "↓"; break;
                    case KeyCode::ArrowLeft: text += ascii ? "left" : "←"; break;
                    case KeyCode::ArrowRight: text += ascii ? "right" : "→"; break;
                    case KeyCode::Enter: text += "enter"; break;
                    case KeyCode::Tab: text += "tab"; break;
                    case KeyCode::Escape: text += "esc"; break;
                    case KeyCode::Home: text += "home"; break;
                    case KeyCode::End: text += "end"; break;
                    case KeyCode::PageUp: text += "pgup"; break;
                    case KeyCode::PageDown: text += "pgdn"; break;
                    case KeyCode::Backspace: text += "backspace"; break;
                    case KeyCode::Delete: text += "delete"; break;
                    case KeyCode::Insert: text += "insert"; break;
                    default:
                        if (key >= KeyCode::F1 && key <= KeyCode::F35)
                            text +=
                                "f" + std::to_string((int)key - (int)KeyCode::F1 + 1);
                        else
                            text += "key";
                }
            }
            else
                text += "key:" + std::to_string(key.code);
        },
        chord.key);
    return text;
}
void validate(Command const& value)
{
    validate_row_text(value.text);
    validate_row_text(value.keywords);
    for (auto const& key : value.keys) {
        validate_row_text(key_label(key));
    }
}
auto command(Command value) -> std::shared_ptr<Command>
{
    validate(value);
    return std::make_shared<Command>(std::move(value));
}
auto command_keys(Command const& value, bool ascii) -> std::string
{
    auto result = std::string{};
    for (auto const& key : value.keys) {
        if (!result.empty()) { result += "/"; }
        result += key_label(key, ascii);
    }
    return result;
}
auto invoke(Command const& value) -> EventResponse
{
    if (!value.enabled || !value.action) { return {}; }
    auto action = value.action;
    return combine({.handled = true}, action());
}
auto dispatch_commands(std::span<std::shared_ptr<Command> const> commands,
                       KeyEvent const& event) -> EventResponse
{
    if (event.action != KeyAction::Press) { return {}; }
    for (auto const& value : commands) {
        if (!value || !value->enabled || !value->action) { continue; }
        for (auto const& key : value->keys) {
            if (event.matches(key.key, key.modifiers)) { return invoke(*value); }
        }
    }
    return {};
}
}  // namespace ox
