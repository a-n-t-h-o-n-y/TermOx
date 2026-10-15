# Shared commands

`<ox/command.hpp>` defines ordinary command data. `command({...})` validates that
data and returns `std::shared_ptr<Command>`. Widgets share this object, so their
labels, keys, enabled/checked state, keywords, and actions have one source.

```cpp
auto save = ox::command({
    .text = "Save",
    .keywords = "write document",
    .action = [&] { save_document(); return ox::EventResponse{.repaint=true}; },
    .keys = {{U's', {.ctrl=true}}},
});
auto menu = ox::Menu{{{.command=save}}};
auto palette = ox::CommandPalette{{save}};
auto help = ox::KeyHelp{{save}};
```

`MenuItem` contains a command pointer and optional children. A null pointer makes a
separator. Submenu headings use ordinary commands with no action. Menu shortcut
labels are generated with `command_keys`, and those same keys dispatch the action.
MenuBar dispatches commands while closed or open, including nested menu commands.
Disabled submenu headings suppress their descendants' menu bindings.

KeyHelp's `bindings` is a vector of command pointers. Commands without actions are
help-only; disabled commands are omitted. CommandPalette searches text and keywords,
preserves original indices in its match list, and reads enabled state at execution.
`invoke(*command)` and `dispatch_commands(commands,event)` return EventResponse;
dispatch only accepts presses and stops at the first enabled matching action.

Commands belong to the application thread. After editing public command data, call
`refresh()` on affected views to update labels, cached choices, and geometry. No
registry, string identifiers, or hidden subscriptions are installed. Put KeyHelp or
MenuBar in the widget tree to enable their scoped shortcuts. Application modal
eligibility still applies; sharing the command between both views does not execute
a key twice. A palette does not install global bindings by itself.

Actions are copied before invocation/activation notifications, allowing callbacks to
replace bindings or destroy the originating view. Keep mutable application state in
explicit captures or tracked widget arguments. Actions return repaint/quit effects;
menu clicks and palette submissions apply those effects even if a notification closes
or destroys the source. An effects handler becomes inert after its event loop dies.

Changing `checked` does not invoke an action, and activating does not automatically
toggle it. The application owns that state, just as it owns enabled state.
