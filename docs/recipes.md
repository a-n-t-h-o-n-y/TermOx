# Recipes

These examples combine widgets into application tasks. Start with
[Core concepts](core-concepts.md) for ownership and connections.

- [Forms](#forms)
- [Background work](#background-work)
- [File selection](#file-selection)
- [Dialogs](#dialogs)
- [Shared commands](#shared-commands)
- [Sorting and filtering a table](#sorting-and-filtering-a-table)

## Forms

`Form` aligns labels, editors, help, and errors. This form requires a name and an
integer voice count. Submit with Ctrl+Enter or Save:

```cpp
#include <ox/ox.hpp>

int main()
{
    auto form = ox::Form{
        std::tuple{ox::LineEdit{}, ox::IntegerEdit{{.value = 8}}},
        std::vector<ox::FormField>{
            {.label = "Name", .help = "Required",
             .validate = [](ox::Widget const& field) -> std::optional<std::string> {
                 auto const& name = static_cast<ox::LineEdit const&>(field);
                 if (name.text().empty()) return "Enter a name";
                 return std::nullopt;
             }},
            {.label = "Voices", .help = "Use a whole number"},
        },
    };
    auto save = ox::Button{{.label = {.text = "Save"}}};
    auto status = ox::Label{"Edit the fields, then save."};
    auto root = ox::Column{std::tie(form, save, status)};
    ox::Connection{.signal = save.on_activate,
                   .slot = [](auto& form) { form.submit(); }}(form);
    ox::Connection{.signal = form.on_submit,
                   .slot = [](ox::Label& status) { status.set_text("Saved"); }}(status);
    return ox::Application{root}.run();
}
```

The validator receives the field widget; the cast matches the widget at that
position. Form commits numeric drafts before validation, so read typed values with
`value()` rather than parsing display text. Invalid drafts produce a field error
and prevent submission. Successful commits remain even if another field fails.

After checking every field, Form stores errors, focuses the first invalid field,
and emits `on_invalid` or `on_submit`. Save application data from `on_submit`.
A cross-field validator may inspect tracked views of other fields but should not
modify widgets, alter field configuration, or submit the form again.

For custom composites, `FormField::commit` overrides automatic draft commit.
Keep field metadata and child counts equal. Put large forms in a ScrollView.
See [Form](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_form.html) and [FormField](https://a-n-t-h-o-n-y.github.io/TermOx/structox_1_1_form_field.html) for commit callbacks and layout options.

## Background work

Workers should produce owned data and post an `event::Custom` action through
`Application::post`. Do not call widget methods, emit their signals, or mutate their
fields from a worker. This example reads the file named by the first argument
(defaulting to `README.md`):

```cpp
#include <ox/ox.hpp>
#include <fstream>
#include <thread>

int main(int argc, char** argv)
{
    auto path = std::string{argc > 1 ? argv[1] : "README.md"};
    auto text = ox::TextBox{{.placeholder = "Reading…", .read_only = true}};
    auto close = ox::Button{{.label = {.text = "Close"}}};
    auto root = ox::Column{std::tie(text, close)};
    auto app = ox::Application{root};
    ox::Connection{.signal = close.on_activate, .slot = [&app] { app.quit(); }}();

    auto worker = std::jthread{[&app, target = ox::track(text), path](std::stop_token stop) {
        auto input = std::ifstream{path};
        auto result = std::string{};
        if (!input) result = "Cannot open " + path;
        for (std::string line; !stop.stop_requested() && std::getline(input, line);)
            result += line + '\n';
        if (stop.stop_requested()) return;
        app.post(ox::event::Custom{.action = [target, result = std::move(result)] {
            if (auto* text = target.try_get())
                text->set_text(result.empty() ? "(empty file)" : result);
            return ox::EventResponse{};
        }});
    }};
    auto code = app.run();
    worker.request_stop();
    return code;
}
```

The posted callback owns the string. Its tracked view follows widget moves and
skips a destroyed editor. `set_text` requests layout and paint, so no additional
repaint effect is needed.

The worker borrows `app`. It is declared after the application so its `jthread`
destructor joins before the application is destroyed. **Every producer must stop
posting before the application dies.** Cancellation here is checked between lines;
it does not interrupt a blocked read.

Posting wakes an idle event loop. Callbacks run on the application thread and must
finish quickly; keep blocking work on the worker. Posts made during a dispatch batch
wait for the next step. See [event-loop internals](internals/event-loop.md) for queue
transfer and waiting behavior.

## File selection

`FilePicker` lists directories and validates selections asynchronously. The
application handles file I/O:

```cpp
#include <ox/ox.hpp>

int main()
{
    auto picker = ox::FilePicker{};
    auto status = ox::Label{"Choose a file"};
    auto root = ox::Column{std::tie(picker, status)};
    ox::Connection{
        .signal = picker.on_choose,
        .slot = [](std::filesystem::path const& path, ox::Label& status) {
            status.set_text(path.string());
        },
    }(status);
    return ox::Application{root}.run();
}
```

`on_choose` supplies a normalized absolute path; `on_cancel` asks the host to cancel.
For a modal chooser, put the picker inside a [Dialog](#dialogs) and connect these
signals to close it. Neither closes the surrounding dialog automatically.

Set `.mode` to a `FilePickerMode` value:

| Mode | Accepted selection |
| --- | --- |
| `OpenFile` | An existing regular file |
| `SaveFile` | An existing file or a new name in an existing directory |
| `Directory` | The selected directory, or the current directory if none is selected |

Save mode does not create or truncate files. The application handles saving and
overwrite confirmation. Set `show_hidden` for dot-prefixed entries or provide a
file `filter`; directories remain available. Call `reload()` after changing configuration.

`set_directory`, `reload`, and `accept` report whether work **started**, not whether
it succeeded. Observe `on_state` and `on_error` for completion. `cancel_loading()`
or Escape cancels pending work; superseded and cancelled results are not applied.
Cancellation is cooperative between filesystem calls, so worker captures must stay
valid until the worker exits.

`filter` and `loader` run on workers: they must be thread-safe and must not access
widgets. Results and signals are delivered on the application thread by an attached
timer. Detached or headless use must call `poll()`. See [FilePicker](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_file_picker.html) for custom
loaders and navigation behavior.

## Dialogs

Keep popups and dialogs in the owner's widget tree. Connect an action to `show()`:

```cpp
auto root = ox::Column{
    ox::Button{{.label = {.text = "Open"}}},
    ox::Dialog{ox::Column{ox::Label{"Enter a name"}, ox::LineEdit{}}, "Name"},
};
auto& [open, dialog] = root.children;
dialog.requested_size = ox::Area{40, 7};
ox::Connection{.signal = open.on_activate,
               .slot = [](auto& dialog) { dialog.show(); }}(dialog);
```

A `Popup` owns its content and borrows a tracked anchor. It can appear below or
beside an anchor, or centered on screen. Popups are modal and dismissible by default;
set `modal = false` for a nonmodal popover.

`Dialog` confines Tab navigation to its content and rejects on Escape. Add action
buttons connected to `accept` or `reject`, then handle `on_result`. A modal popup
confines input, focus, and widget timers to its subtree. Closing it restores the
preceding eligible focus. See [Popup](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_popup.html) and [Dialog](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_dialog.html) for placement and dismissal.

## Shared commands

A shared `Command` supplies labels, shortcuts, enabled/checked state, and actions
for menus, palettes, and keyboard help. For an application function `save_document()`:

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

Include MenuBar or KeyHelp in the widget tree to enable scoped shortcuts. A palette
does not install global bindings or an opening shortcut. Disabled commands cannot
execute; commands without actions serve as submenu headings or help-only entries.
A null menu command makes a separator.

```cpp
auto open = ox::command({.text="Open", .action=[] { return ox::EventResponse{}; },
                         .keys={{U'o', {.ctrl=true}}}});
auto bar = ox::MenuBar{{{"File", {{.command=open}}}}};
```

F10 focuses the menu bar; Alt plus a menu label's first letter opens it. The menu
bar dispatches command shortcuts whether its menus are open or closed.

After editing shared command data, call `refresh()` on affected views. The
application owns checked/enabled state; activating an action does not toggle it
automatically. Actions return repaint/quit effects. They are copied before
notifications, so a callback may close or destroy the source view; captured
application state must still outlive the invocation.

### Command palettes and keyboard help

Typing in a palette's public `query` editor filters by text and keywords. Enter
executes the selected command. `matches()` refers to original command indices.
Put a palette in a [Dialog](#dialogs) for an overlay and connect `on_execute` to
close the dialog.

KeyHelp displays the same commands and dispatches their shortcuts. Set
`expanded = true` for wrapped help. Sharing a command between KeyHelp and MenuBar
does not execute it twice for one key event. Modal input eligibility still applies.
See [Command](https://a-n-t-h-o-n-y.github.io/TermOx/structox_1_1_command.html), [CommandPalette](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_command_palette.html), and [KeyHelp](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_key_help.html) for command details.

## Sorting and filtering a table

The default table sort is lexical. Supply a comparator for numeric values:

```cpp
#include <ox/ox.hpp>
using namespace ox;

int main()
{
    auto table = DataTable{{
        .columns = {{.heading = "Name", .size = SizePolicy::min(16)},
                    {.heading = "Count", .size = SizePolicy::fixed(8),
                     .align = Align::Right}},
        .rows = {{"界 é", "10"}, {"Sample", "2"}}}};
    table.select(0);
    table.sort_rows(1, true, [](std::string_view a, std::string_view b) {
        return std::stoi(std::string{a}) < std::stoi(std::string{b});
    });
    return Application{table}.run();
}
```

Stable sorting preserves the selected row and reveals it. Comparators must provide
a strict weak ordering and must not mutate the table. Header clicks emit
`on_header(column)`; connect that signal to `sort_rows` to enable click-to-sort.

Keep a source dataset in application state. To filter it, construct matching rows
and pass them to `set_rows`. Replacement clears selection and vertical scroll.
If selection must survive filtering, retain a record ID in application state and
find its new view index after replacement. Table indices are not permanent IDs.

Add [scrollbars](layout.md#link-a-scrollbar) for large views.
See [DataTable](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_data_table.html) for updates and sorting, and the [Showcase](../examples/showcase.md)
for filtering, header sorting, and linked scrolling together.
