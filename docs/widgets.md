# Widgets

Include `<ox/ox.hpp>` for these examples. Follow the API links for all options,
methods, and signals. [Layout](layout.md) covers containers and scrollbars;
[Appearance](appearance.md) covers styling.

| Need | Widget |
| --- | --- |
| Display text | [Label](#labels) |
| Trigger an action or select an option | [Button, CheckBox, RadioGroup](#buttons-and-choices) |
| Edit text | [LineEdit, TextBox](#text-editing) |
| Enter a number | [Slider, NumericEdit, IntegerEdit](#numeric-input) |
| Choose from a list | [ListView, Select, ComboBox](#lists-and-dropdowns) |
| Display structured data | [DataTable, TreeView](#tables-and-trees) |
| Show a stream of messages | [LogView](#logs) |
| Show progress | [ProgressBar, Spinner](#progress-and-activity) |
| Open a dialog, file picker, menu, or palette | [Recipes](recipes.md) |
| Display pixels or draw custom content | [Custom widgets](custom-widgets.md) |
| Draw fine lines with Braille dots | [DotGrid](custom-widgets.md#braille-dots) |

## Labels

```cpp
auto heading = ox::Label{{.text = "Heading", .align = ox::Align::Start}};
auto side = ox::Label{{
    .text = "Side", .vertical = true, .align = ox::Align::End,
}};
```

[Label](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_label.html) displays text horizontally, or one grapheme per row with `vertical = true`.
`Start`, `Center`, and `End` align along that direction. Text clips at grapheme
boundaries, preserving combining sequences and emoji.

Use `set_text` to update content and `set_vertical` to change orientation. Horizontal
labels default to one row; vertical labels reserve their natural width and flexible
height. Explicit size policies override those defaults. Give vertical button or
checkbox captions enough height in their containing control.

## Buttons and choices

[Button](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_button.html) emits `on_activate` for keyboard or mouse activation. See
[Connections](core-concepts.md#connections-and-lifetime) for a complete example.
Use `on_activate` for actions; `on_press` and `on_release` describe mouse gestures.

```cpp
auto notifications = ox::CheckBox{{.checked = true,
                                    .label = {.text = "Notifications"}}};
notifications.set_checked(false);
```

[CheckBox](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_check_box.html) and [Switch](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_switch.html) hold a boolean. Use `set_checked` or `toggle`, and
observe `on_change` for changes. For one choice from a list, use [RadioGroup](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_radio_group.html):

```cpp
auto quality = ox::RadioGroup{{.items = {"Draft", "Normal", "High"},
                                .selected_index = 1}};
quality.select(2);
```

`select(std::nullopt)` clears the choice. Replacing items clears selection, so
restore it explicitly when needed. RadioGroup supports vertical scrollbars.

## Text editing

Use [LineEdit](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_line_edit.html) for one line and [TextBox](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_text_box.html) for multiple lines:

```cpp
#include <ox/ox.hpp>

int main()
{
    auto editor = ox::LineEdit{{.placeholder = "Enter a name"}};
    auto status = ox::Label{"Press Enter to submit"};
    auto root = ox::Column{std::tie(editor, status)};
    ox::Connection{
        .signal = editor.on_submit,
        .slot = [](std::string const& text, ox::Label& status) {
            status.set_text("Hello, " + text);
        },
    }(status);
    return ox::Application{root}.run();
}
```

Use `set_text` to replace a document and reset selection/history; `insert` retains
undo history. `read_only` blocks user edits while allowing selection, copying, and
programmatic changes. Single-line input removes tabs and line breaks.

Caret and anchor positions are **UTF-8 byte offsets at grapheme boundaries**, not
character counts or terminal columns. `editor().selection()` gives the sorted byte
range; `on_selection_change` preserves the selection's direction.

`accept_character` filters typed or pasted scalars. A whole-value `validator` runs
on submission, allowing intermediate editing states. LineEdit submits on Enter;
TextBox submits on Ctrl+Enter. Successful validation emits `on_submit`, rejection
emits `on_invalid`. Copy signal text if application state needs to retain it.

Ctrl+C/X/V use the application's clipboard, which defaults to in-memory storage.
Replace `app.clipboard.read` and `.write` for a host clipboard; callbacks run on the
application thread. For line numbers and selection colors, see
[editor appearance](appearance.md#editors).

## Numeric input

Use `Slider` for a range, `NumericEdit` for decimal input, and `IntegerEdit` for whole
numbers:

```cpp
#include <ox/ox.hpp>

int main()
{
    auto slider = ox::Slider{{.range = {0, 1, .05}, .value = .5}};
    auto number = ox::NumericEdit{{.range = {0, 1, .05}, .value = .5}};
    auto root = ox::Column{std::tie(slider, number)};
    ox::Connection{.signal = slider.on_change,
                   .slot = [](double value, ox::NumericEdit& n) {
                       n.set_value(value);
                   }}(number);
    return ox::Application{root}.run();
}
```

Ranges have inclusive bounds and a positive step. Programmatic values clamp to the
range without snapping to a step. Use `value()` and `set_value`; `on_change` reports
changes. `NumericEdit` and `IntegerEdit` are specializations of `BasicNumericEdit`.
See [Slider](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_slider.html) and [BasicNumericEdit](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_basic_numeric_edit.html) for range and editing options.

### Drafts and formatting

```cpp
auto count = ox::IntegerEdit{{.range = {0, 1000, 1}, .value = 3}};
auto weight = ox::NumericEdit{{
    .range = {0, 100, .5}, .value = 12.345,
    .format = "{:.2f}", .suffix = " kg",
    .commit_on_focus_loss = true,
}};
```

Typing creates a draft. Enter or `commit()` accepts a number in range; invalid text
stays visible and leaves the committed value unchanged. Escape or `revert()` discards
the draft. `set_value` replaces it. Focus loss preserves drafts unless
`commit_on_focus_loss` is enabled; invalid input still remains visible.

Read typed values after committing, not by parsing display text. [Forms](recipes.md#forms)
commit numeric drafts before validation. Formatting applies to unfocused committed
values; focusing exposes full precision. Prefixes and suffixes are decorations.
Use `L`, as in `"{:.2Lf}"`, for locale-aware display.

## Lists and dropdowns

[ListView](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_list_view.html) shows visible choices. Enable `multiple` for range and toggle selection:

```cpp
auto list = ox::ListView{{
    .items = {{"Alpha"}, {"Unavailable", false}, {"Beta"}, {"Gamma"}},
    .multiple = true,
}};
list.select(0);
list.select(3, ox::SelectionAction::Extend);
list.on_select.connect([](std::vector<std::size_t> const& indices) {
    // A stable value snapshot; labels need not be unique.
});
```

Shift extends a range, Ctrl-click/Ctrl-Space toggles, and Ctrl-A selects eligible rows.
Selected indices refer to the current model, not permanent IDs. `set_items` clears
selection; incremental insert/erase operations preserve unaffected item identity.
Disabled items cannot be selected. Manual scrolling does not change selection.

```cpp
auto choice = ox::Select{{.items = {{"Red"}, {"Green"}, {"Blue"}}, .selected = 1}};
auto editable = ox::ComboBox{{
    .items = {{"Apple"}, {"Apricot"}, {"Banana"}},
    .editor = {.placeholder = "Type a value"},
}};
```

[Select](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_select.html) commits a choice when activated; Escape, Tab, or an outside click cancels
the preview. [ComboBox](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_combo_box.html) also accepts arbitrary text and filters choices while
typing. Its `on_change` reports text, while `on_select` reports item identity.
Editing a committed label clears that identity. Replace items through the owning
control's `set_items`, and use its public components for styling.

## Tables and trees

[DataTable](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_data_table.html) displays read-only cells with selectable rows and fixed headers.
Configure columns and supply rectangular rows of UTF-8 strings. See
[sorting and filtering](recipes.md#sorting-and-filtering-a-table) for a complete example.
Use `set_rows` for replacement or `set_cell`/`insert_row`/`erase_row` for incremental
updates. Replacement clears selection; incremental edits preserve unaffected rows.

[TreeView](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_tree_view.html) displays nested choices:

```cpp
#include <ox/ox.hpp>

int main()
{
    auto tree = ox::TreeView{{.nodes = {
        {"Project", true, true,
         {{"Sources", true, false, {{"main.cpp"}}}, {"README.md"}}}
    }}};
    tree.select({0, 0, 0}); // Expand ancestors and select main.cpp.
    return ox::Application{tree}.run();
}
```

A path is a sequence of child indices within the current model. `select(path)`
expands ancestors. Replacing nodes clears selection; collapsing a branch hides
selected descendants and moves a descendant cursor to the parent. Disabled nodes
disable their descendants. Right expands or enters a branch; Left collapses or
selects its parent. Multiple selection uses the list's Shift/Ctrl controls.

## Logs

```cpp
auto log = ox::LogView{};
log.set_max_lines(1000);
log.append("Connected\nWaiting for events\n");
```

[LogView](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_log_view.html) retains a bounded number of lines and discards the oldest when full.
`append` accepts newline-separated UTF-8 records; a trailing newline adds no extra
record. Logs follow their tail until the user scrolls upward. Call
`set_follow_tail(true)` to resume. While paused, appends preserve the viewport and
selection relative to retained text.

The public read-only `editor` supplies selection, copying, wrapping, and styling.
Append on the application thread; [post results](recipes.md#background-work) from
workers. Logs and trees support [linked scrollbars](layout.md#link-a-scrollbar).

## Progress and activity

[ProgressBar](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_progress_bar.html) displays a value without accepting focus. `show_percentage` toggles
its centered label. [Spinner](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1_spinner.html) displays animated frames; `set_running` starts or
stops it. Stopped spinners have no active timer. [Motion settings](appearance.md#motion)
control themed animation without delaying application state changes.
