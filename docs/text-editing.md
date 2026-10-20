# Edit text

Use `LineEdit` for one line and `TextBox` for multiple lines. Both own normalized
UTF-8 text, selection, undo history, and a viewport.

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

## Editing commands

Both widgets expose `text()` and a read-only `editor()` view. Use `set_text`,
`set_caret`, `select_all`, `insert`, `undo`, `redo`, or `edit(callback)` for mutations.
`edit` stages a batch before committing; changing the single/multiline mode is
rejected. Commands request their layout/repaint effects and coalesce notifications.

`read_only` blocks user document edits while preserving navigation, selection,
copying, and programmatic changes. `set_text` normalizes a new value and resets
selection/history; `insert` retains undo history. Single-line input removes tabs
and line breaks. Invalid UTF-8 and unsupported controls are rejected.

## Carets and selection

Caret and anchor positions are UTF-8 byte offsets at grapheme boundaries. They are
not scalar counts or terminal columns. `on_selection_change(anchor, caret)` preserves
selection direction; a backward selection has `caret < anchor`. Use
`editor().selection()` for the sorted half-open byte range.

Mouse input supports Shift-click, double-click words, triple-click lines, and
captured dragging. Arrow keys navigate; Shift extends selection. TextBox supports
wrapping and vertical navigation. Its scroll position can change independently of
the caret. [Link scrolling](scrolling.md) adds scrollbars to either editor.

## Filtering and submission

`accept_character(char32_t)` filters typed or pasted scalars. A whole-value
`validator(string_view)` runs on submission and allows intermediate editing states.
Return true to accept the complete value; an empty validator accepts all values.

LineEdit submits on Enter and TextBox on Ctrl+Enter. Successful validation emits
`on_submit`; rejection emits `on_invalid`. Key repeats do not submit.
`on_change(string const&)` carries a stable snapshot during signal delivery; copy it
if application state must retain it. See [form validation](forms.md) to combine
several fields and commit numeric drafts before cross-field checks.

## Clipboard and presentation

Ctrl+C/X/V use the attached application's clipboard, which defaults to in-memory
storage. Replace `app.clipboard.read` and `.write` for a host clipboard. Callbacks
run on the application thread; empty callbacks and detached clipboard access throw.
Read-only editors allow copying, but do not cut or paste into the document.

`style`, `placeholder`, `align`, and selection patches configure appearance. Use
[editor presentation](presentation.md#textbox-and-lineedit) for line numbers,
current-line highlighting, focus patches, and inactive selections. Add outer spacing
and outlines with [containers](containers.md) and [decorations](decorations.md).

## Editing core

For custom bindings or a non-widget editor, use `TextEdit`, `TextLayout`, and the
functions in `<ox/text_view.hpp>`. Their navigation, streaming edits, cached analysis,
width policy, and selection painting are described in
[text editing internals](internals/text.md).
