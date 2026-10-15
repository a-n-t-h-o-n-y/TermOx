#pragma once
#include <memory>
#include <ox/core/events.hpp>
#include <span>
#include <string>
#include <vector>
namespace ox {
struct KeyChord {
    Key key = KeyCode::Enter;
    Modifiers modifiers;
};
/** Shared application-thread command data. Refresh views after changing presentation.
 */
struct Command {
    std::string text;
    std::string keywords = {};
    bool enabled = true;
    std::function<EventResponse()> action = {};
    std::vector<KeyChord> keys = {};
    std::optional<bool> checked = {};
};
auto command(Command) -> std::shared_ptr<Command>;
void validate(Command const&);
auto key_label(KeyChord const&, bool ascii = false) -> std::string;
auto command_keys(Command const&, bool ascii = false) -> std::string;
/** Presses only; first enabled matching action wins. Actions are snapshotted. */
auto dispatch_commands(std::span<std::shared_ptr<Command> const>, KeyEvent const&)
    -> EventResponse;
auto invoke(Command const&) -> EventResponse;
}  // namespace ox
