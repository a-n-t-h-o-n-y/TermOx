# Values and composite widgets

Include `<ox/value.hpp>` for Slider, NumericEdit, ProgressBar, Spinner, and Switch.
The other widgets have their own headers; `<ox/ox.hpp>` includes everything.

## Values and status

`ValueRange{.minimum=0, .maximum=100, .step=1}` uses finite inclusive bounds and
positive increments. `validate`, `clamp_value`, and `value_fraction` are independent
functions. Ranges may be degenerate. Programmatic values clamp without quantization;
invalid ranges and non-finite values throw. `range()` and `value()` are read-only;
`set_range` and `set_value` notify `on_change(double)` only when the value changes.

```cpp
auto slider = ox::Slider{{.range = {0, 1, .05}, .value = .5}};
auto number = ox::NumericEdit{{.range = {0, 1, .05}, .value = .5}};
ox::Connection{.signal = slider.on_change,
               .slot = [](double value, ox::NumericEdit& n) { n.set_value(value); }}(number);
```

Slider supports Left/Right, Up/Down, Home/End, and PageUp/PageDown (ten increments).
Left/Down decrease; Right/Up increase. Vertical sliders place their maximum at the top.
Pointer dragging captures the left button and clamps outside the track.

`BasicNumericEdit<T>` stores, accepts, and emits exactly `T`. `NumericEdit` is the
`double` alias; `IntegerEdit` is the `int` alias. `BasicValueRange<T>` keeps bounds
and step in the same type. Integer parsing rejects fractions and overflow without
conversion through floating point; stepping saturates safely at the bounds.

```cpp
auto count = ox::IntegerEdit{{.range = {0, 1000, 1}, .value = 3}};
auto weight = ox::NumericEdit{{
    .range = {0, 100, .5}, .value = 12.345,
    .format = "{:.2f}", .suffix = " kg",
    .commit_on_focus_loss = true,
}};
count.on_change.connect([](int value) { /* value is an int */ });
```

The default format is `"{}"`; prefix and suffix default to empty strings. Prefix/suffix
are separate, noneditable decorations, measured in terminal columns and clipped to the
available space. Numeric labels also accept `.prefix` and `.suffix` alongside `.format`.
Use standard format specifications, including `L` for locale-aware display. The editor's
`.locale` defaults to `std::locale::classic()` and controls parsing and round-trip editing
text. For example, `.format = "{:.2Lf}"` uses the configured locale's decimal separator.
Formats must produce printable single-line text. Invalid format strings throw.

NumericEdit owns a public LineEdit-compatible `editor`. Text edits are staged: Enter or
`commit()` accepts a finite number in range; invalid text remains visible, emits
`on_invalid(string const&)`, and leaves the value unchanged. `commit()` reports validity,
including an unchanged value. `has_draft()` distinguishes staged text from display text.
`value()`, `set_value(T)`, and `on_change(T)` use the stored type. Up/Down steps from the
committed value; Escape or `revert()` discards the draft. Programmatic `set_value` replaces
any draft. Use `editor` for styling, selection, validation, and clipboard behavior.

Unfocused committed values use `.format`. Focusing an unchanged field exposes full
round-trip precision, so a format like `"{:.2f}"` cannot silently round the stored value.
Draft text remains exactly as typed until commit/revert. Focus loss preserves drafts by
default; `.commit_on_focus_loss = true` attempts a commit instead. Invalid drafts stay
visible. Public presentation changes should be followed by `refresh()`.

ProgressBar is unfocusable. `show_percentage` controls its centered label. Spinner
animates printable UTF-8 frames at a positive timer interval; `set_running`, `tick`,
and `set_frames` provide explicit control. Stopped spinners have no active timer;
application timer eligibility handles hidden/disabled widgets and modal scopes.
`on_change(bool)` reports running changes. Switch shares CheckBox's entire boolean
API and signals with an alternate indicator; its checked state is private.

## TreeView

The model is ordinary recursive data:

```cpp
auto tree = ox::TreeView{{.nodes = {
    {"Project", true, true, {{"Sources", true, false, {{"main.cpp"}}}, {"README.md"}}}
}}};
tree.select({0, 0, 0}); // expands ancestors, then selects the leaf
```

`TreeNode` contains `text`, `enabled`, `expanded`, and `children`. Paths are vectors of
child indices. `nodes()` and `list()` expose const views. `set_nodes` validates all labels
and replaces the model, clearing selection. `set_expanded(path,bool)` preserves remaining
selected paths; collapsing hides descendant selections and moves a descendant cursor
to the parent. `select(path,SelectionAction)` expands ancestors. Disabled nodes disable
all descendants. Multiple selection uses ListView's Shift/Ctrl behavior.

`selected_paths()` returns selected path values; `current_path()` returns the optional
cursor path. `on_select(vector<vector<size_t>> const&)`, `on_activate(path const&)`, and
`on_expand(path const&,bool)` deliver stable snapshots. Enter/double-click activates and
toggles branches. Right expands or enters a branch; Left collapses or selects its parent.
Scrolling, Unicode painting, typeahead, and ordinary navigation come from ListView.
Paths belong to the current model, rather than providing persistent identity across
replacement. Model replacement from a callback cancels pending path selection and
activation; nested rebuilds retain the latest model and selection.

## LogView

LogView owns a public read-only TextBox `editor` and a bounded deque of lines. `append`
accepts printable UTF-8 records separated by newlines; each retained record ends in a newline.
A trailing newline does not append an extra blank record; `append("")` appends one blank line.
Invalid input rejects the whole batch. `max_lines` is positive and defaults to 10,000;
`set_max_lines` trims oldest records. Reducing capacity preserves a paused viewport
relative to retained text, counting wrapped display rows at the current width.
Tail-following logs stay at the end. Editing history is disabled. Appends reuse retained
text/layout analysis through `TextBox::append_lines`, including measurement in layouts;
batches larger than capacity retain only their newest records.

`set_follow_tail(true)` reveals the newest content. Manual upward scrolling pauses
following; `set_follow_tail(false)` leaves the viewport alone. Appending preserves selection
and the retained viewport when paused, adjusting byte offsets as old records are removed.
`on_append(size_t)` reports records accepted, `on_follow(bool)` reports follow changes.
`clear()` removes all records. Text selection/copy, two-axis scrolling, and wrapping are
provided by the editor. Producers should post append actions to the application thread.

## SplitPane

`SplitPane{first,second,.5}` owns two arbitrary widgets. Public `vertical=false` places
panes side by side; true stacks them. `ratio()` is a finite fraction clamped to [0,1];
`set_ratio` emits `on_change(double)` only on changes. Resizing preserves the requested
ratio. `first_minimum` and `second_minimum` constrain actual layout; when they cannot
both fit, available space is shared proportionally. The divider occupies one cell.
Drag it with the left button, or focus it and use arrows, Home, and End. Child input
continues to route normally. Pane state and focus survive resizing.

## Forms

Form owns a tuple or sized forward range of widgets, plus a same-length vector of
`FormField{label,help,validate}`. Fields remain directly accessible in `children`.
Labels occupy a shared column; `label_width` optionally fixes it. Help/errors appear
beneath each field; `gap` separates rows. Field width policies limit controls within
the shared field column and also constrain measurement of wrapped content. Put large
forms inside ScrollView.

```cpp
auto form = ox::Form{std::tuple{ox::LineEdit{}},
    std::vector<ox::FormField>{{"Name", "", [](ox::Widget const& widget)
        -> std::optional<std::string> {
        auto const& input = static_cast<ox::LineEdit const&>(widget);
        return input.text().empty() ? std::optional<std::string>{"Required"} : std::nullopt;
    }}}};
```

Validators are functions taking the actual field widget and returning an error message
or nullopt. `submit()` validates every field, stores `errors()`, focuses the first invalid
field, and emits `on_invalid(errors const&)` or `on_submit()`. Ctrl-Enter submits from
within the form. Before validation, Form calls `commit()` on fields whose concrete types expose a
boolean commit operation, including every BasicNumericEdit specialization. Invalid drafts
produce an `"Invalid value"` field error and prevent submission. Validation then sees the
committed typed value. Each `FormField` also accepts an optional `commit(Widget&) -> bool`
callback, overriding automatic commit; use it for custom composites, fields stored through
base-class references, or an explicit no-op when preserving drafts is desired. Submission
is not atomic across fields: successful commits remain if another field is invalid.
`set_fields` validates labels/count and resets errors. Validate functions must not alter
field configuration while running.

## FilePicker

FilePicker is ordinary content; use `Dialog{FilePicker{...},"Open"}` for a modal chooser.
It owns public `location`, `filename`, `up`, `choose`, and `cancel` controls. `entries()`
and `paths()` are const aligned views. Directories sort first; dot-prefixed names are
hidden unless `show_hidden=true`. A functional `filter(directory_entry const&)` filters
files, never directories. Filters run on the worker thread. Call `reload()` after
changing the public configuration.

`FilePickerMode::OpenFile` requires an existing regular file. `SaveFile` accepts an
existing regular file or a new name in an existing directory. `Directory` returns the
selected directory, or the current directory when nothing is selected. Save mode never creates/truncates anything; the application decides
whether to ask about overwriting and performs the save. Symlink targets are resolved.
`on_choose(path const&)` returns a normalized absolute path; `on_cancel()` asks the owning
application/dialog to cancel. Choosing does not close the surrounding dialog automatically.

Enter in the location navigates; Enter/double-click on a directory opens it; activating
files or entering a filename chooses. Up/Alt-Up goes to the parent; F5 reloads.
Failed navigation preserves the previous model and emits `on_error(string const&)`;
`error()` is also painted. Navigation and file validation run on worker threads.
`set_directory`, `reload`, and `accept` return whether work started, rather than its
final success. Observe `on_state(FilePickerState)` and `on_error` for completion.
`is_loading()` covers `Loading` and `Validating`; other states are `Idle`, `Ready`,
`Error`, and `Cancelled`. The footer shows loading/checking status, and choosing is
disabled during work. The previous model remains visible until a successful load.

`cancel_loading()` or Escape cancels work without waiting for filesystem calls.
New requests supersede old ones; results from cancelled or superseded workers are
never applied. Destruction and move assignment cancel the old request; moves retain
active work. Cancellation is cooperative between filesystem calls, so a blocked OS
call may continue on its worker until it returns. Worker captures remain alive until
that worker exits.

All widget mutation and signals run on the application thread. An attached timer
delivers completed results and stops while idle. Detached/headless use calls `poll()`
explicitly. Filters and the optional `loader(path,stop_token)` must be thread-safe
and must not access widgets. The default pure `read_directory` returns a plain
`DirectoryListing` with a canonical directory, directory entries, and an error string.
A custom loader can provide another filesystem source or deterministic test data.
The loader must honor stop requests between operations and return absolute paths.
File validation also runs asynchronously and never creates or truncates a file.

## CommandPalette

`Command{text,keywords,enabled,action,keys,checked}` is shared command data. Create
one with `command({...})` and pass the resulting `std::shared_ptr<Command>` to any
number of menus, palettes, and KeyHelp widgets. CommandPalette owns
a public LineEdit `query` and const `choices()`/`matches()` views. Matching uses the pure
`command_score(query,text)` function: case-insensitive ASCII subsequences, exact non-ASCII
scalars, and spaces ignored in the query. Smaller skipped-character scores rank first;
ties preserve input order. Keywords extend search without changing the visible label.

Up/Down/Page/Home/End navigate while query retains focus; Enter executes the current
choice. Disabled commands remain visible and cannot execute. `execute(original_index)`
copies the action before emitting `on_execute(original_index)`, then calls it; callbacks
may close/destroy the palette without invalidating that copied action. `set_commands`
validates labels and rebuilds matches; `refresh()` reapplies edited command data,
including its cached decoded search text. Each query is decoded once. Equivalent
ranked choices preserve selection and viewport without rebuilding the list.
Action `EventResponse` effects reach the owning application, including repaint/quit
effects after a notification destroys the source widget. See [shared commands](commands.md).
Compose inside Dialog/Popup, and close the host
from `on_execute` if desired. No implicit global shortcut or application state is installed.

## Demo

Build `TermOx.examples.views` and run it. Its tabs show values/status, tree/log panes,
form validation, filesystem browsing, and command search. Tab changes focus, Ctrl-Tab
switches pages, Ctrl-Enter submits a form, and Ctrl-C exits.
