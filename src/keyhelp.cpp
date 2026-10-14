#include <esc/text.hpp>
#include <ox/keyhelp.hpp>
#include <ox/put.hpp>
#include <ox/text_view.hpp>
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
auto dispatch_shortcuts(std::span<Shortcut const> bindings, KeyEvent const& event)
    -> EventResponse
{
    if (event.action != KeyAction::Press) return {};
    for (auto const& binding : bindings) {
        if (!binding.enabled || !binding.action) continue;
        for (auto const& key : binding.keys)
            if (event.matches(key.key, key.modifiers)) {
                auto action =
                    binding.action;  // May replace the bindings or destroy their owner.
                return combine({.handled = true}, action());
            }
    }
    return {};
}
KeyHelp::KeyHelp(std::vector<Shortcut> value)
    : Widget{FocusPolicy::None, SizePolicy::flex(),
             SizePolicy{.minimum = 1, .flexibility = 0}},
      bindings{std::move(value)}
{
    auto lifetime = track(*this);
    shortcuts = [lifetime](KeyEvent const& key) {
        auto* self = lifetime.try_get();
        return self ? self->handle(key) : EventResponse{};
    };
}
auto KeyHelp::handle(KeyEvent const& key) -> EventResponse
{ return dispatch_shortcuts(bindings, key); }
auto KeyHelp::text(bool ascii) const -> std::string
{
    auto result = std::string{};
    for (auto const& binding : bindings) {
        if (!binding.enabled || binding.keys.empty()) continue;
        if (!result.empty()) result += expanded ? "\n" : (ascii ? "  /  " : "  ·  ");
        for (std::size_t i = 0; i < binding.keys.size(); ++i) {
            if (i) result += "/";
            result += key_label(binding.keys[i], ascii);
        }
        result += " " + binding.description;
    }
    return result;
}
auto KeyHelp::measure(Area available, esc::WidthPolicy policy) const -> Area
{
    validate(available);
    return measure_text(text(theme().ascii), available,
                        expanded ? TextWrap::Word : TextWrap::None, policy);
}
auto KeyHelp::paint(Canvas canvas) -> Cursor
{
    auto normal = appearance(style, Surface::Muted);
    fill(canvas, normal);
    layout_.update(text(theme().ascii), canvas.size().width,
                   expanded ? TextWrap::Word : TextWrap::None, canvas.width_policy());
    paint_text(canvas, layout_, Align::Start, {}, normal, normal);
    return {};
}
}  // namespace ox
