#include <esc/text.hpp>
#include <ox/keyhelp.hpp>
#include <ox/put.hpp>
#include <ox/text_view.hpp>
namespace ox {
KeyHelp::KeyHelp(std::vector<std::shared_ptr<Command>> value)
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
{ return dispatch_commands(bindings, key); }
auto KeyHelp::text(bool ascii) const -> std::string
{
    auto result = std::string{};
    for (auto const& binding : bindings) {
        if (!binding || !binding->enabled || binding->keys.empty()) continue;
        if (!result.empty()) result += expanded ? "\n" : (ascii ? "  /  " : "  ·  ");
        result += command_keys(*binding, ascii) + " " + binding->text;
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
