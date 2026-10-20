# Shared commands

`command({...})`, defined in `<ox/command.hpp>`, validates command data and returns
`std::shared_ptr<Command>`. Widgets can share its label, keys, enabled/checked state,
keywords, and action. For an application function `save_document()`:

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
separator. Submenu headings use commands with no action. Menu shortcut
labels are generated with `command_keys`, and those same keys dispatch the action.
MenuBar dispatches commands while closed or open, including nested menu commands.
Disabled submenu headings suppress their descendants' menu bindings.

KeyHelp omits disabled commands and treats commands without actions as help-only.
Shortcut dispatch accepts presses and stops at the first enabled matching action.

## Update shared commands

Commands belong to the application thread. After editing public command data, call
`refresh()` on affected views to update labels, cached choices, and geometry. Put KeyHelp or
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

## Search with a command palette

Pass the same commands to `CommandPalette`, as in the example above. Typing in its
public `query` editor filters choices by text and keywords. Up/Down/Page/Home/End
navigate while the editor retains focus; Enter executes the selected command.
Disabled commands remain visible but cannot execute.

Matching uses subsequences, ignoring ASCII case and query spaces; non-ASCII
characters match exactly. Closer matches rank first, with input order breaking ties.
`matches()` retains original command indices. Use `set_commands` to replace the
commands and `refresh()` after editing shared command data.

For an overlay, put the palette inside a
[Dialog or Popup](selection-and-application.md#root-overlays-popups-and-dialogs).
Connect `on_execute` to close the host if desired. It supplies the original command
index; the action is copied before notification, so closing or destroying the
palette does not invalidate the action or its repaint/quit effects. The palette
does not install a global shortcut to open itself.
