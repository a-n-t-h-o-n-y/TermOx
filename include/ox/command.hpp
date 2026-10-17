/** @file
 * Reusable actions, keyboard shortcuts, and command-backed buttons.
 */
#pragma once
#include <memory>
#include <ox/core/events.hpp>
#include <span>
#include <string>
#include <vector>
namespace ox {
/** One key identity and the exact modifiers used by a command binding. */
struct KeyChord {
    /** Key code, Unicode scalar, or physical key identity; Enter is the default. */
    Key key = KeyCode::Enter;
    /** Required modifier set; unspecified modifiers are false. */
    Modifiers modifiers;
};
/** Shared application-thread command data. Refresh views after changing presentation.
 */
struct Command {
    /** Single-line visible command label; invalid UTF-8 and control characters are rejected. */
    std::string text;
    /** Additional single-line search terms, not part of the visible label. */
    std::string keywords = {};
    /** Whether views and dispatch may execute this command. */
    bool enabled = true;
    /** Optional application-thread action; empty actions are help-only and do not handle
     * keys.
     */
    std::function<EventResponse()> action = {};
    /** Alternative exact-match shortcuts in display and dispatch order. */
    std::vector<KeyChord> keys = {};
    /** Optional checkmark state for command views; invocation does not toggle it
     * automatically.
     */
    std::optional<bool> checked = {};
};
/** Validate and allocate shared command data; views retain shared ownership. */
auto command(Command) -> std::shared_ptr<Command>;
/** Reject labels, keywords, or key labels containing malformed UTF-8 or nonprintable row
 * text.
 */
void validate(Command const&);
/** Format modifiers and a readable key name; ascii replaces arrow glyphs with words. */
auto key_label(KeyChord const&, bool ascii = false) -> std::string;
/** Join alternative shortcut labels with slashes, or return an empty string when none exist. */
auto command_keys(Command const&, bool ascii = false) -> std::string;
/** Presses only; first enabled matching action wins. Actions are snapshotted. */
auto dispatch_commands(std::span<std::shared_ptr<Command> const>, KeyEvent const&)
    -> EventResponse;
/** Copy and invoke an enabled nonempty action; mark the response handled. Disabled/empty
 * commands return no effects.
 */
auto invoke(Command const&) -> EventResponse;
}  // namespace ox
