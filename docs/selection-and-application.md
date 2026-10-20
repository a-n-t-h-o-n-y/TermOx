# Lists, choices, and dialogs

Use `ListView` for visible choices, `Select` for a dropdown, and `ComboBox` when
arbitrary text is also allowed. Include `<ox/ox.hpp>` for the examples below.

## ListView

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

`ListItem` holds text, enabled state, and a StylePatch. `items()` and `selected_rows()`
return const spans; `current_row()` exposes the cursor. Single selection is the default.
Shift extends a range, Ctrl-click/Ctrl-Space toggles, Ctrl-arrows move only the cursor,
Ctrl-A selects all eligible rows, and Space toggles the current row in multiple mode.
Enter and double-click activate once; repeat Enter does not activate. Incremental
prefix search cycles matching rows (ASCII case-insensitive, other Unicode scalars exact).

`on_select` emits only for changes in selected indices; `on_current` reports cursor
changes; `on_activate` carries an index. Callbacks may move or destroy the widget.
Replacing the model during selection cancels a pending activation of its old row.
`set_items` validates before replacement and clears selection. `set_item`, `insert_item`,
and `erase_item` preserve unaffected item identity by shifting indices; disabling an
item removes it from selection. Items must be single-line printable UTF-8.

Navigation and explicit selection reveal the cursor. Manual scrolling leaves the
selection unchanged. Use [linked scrollbars](scrolling.md) for either axis, or
[RadioGroup](controls.md#radiogroup) for a simpler single-choice control.

## Select and ComboBox

```cpp
auto choice = ox::Select{{.items = {{"Red"}, {"Green"}, {"Blue"}}, .selected = 1}};
auto editable = ox::ComboBox{{
    .items = {{"Apple"}, {"Apricot"}, {"Banana"}},
    .editor = {.placeholder = "Type a value"},
}};
```

Open a Select by activating its button or pressing Alt-Down/F4. Navigation previews
a choice; activation commits its index and emits `on_change`. Escape, Tab, or an
outside click cancels the preview. Tab then advances through the surrounding form.
Disabled choices cannot be committed, and duplicate captions keep distinct indices.
Use `select` and `set_items` to change state; `selected_index` and `items` read it.
The owned `button` and `popup` expose presentation options.

ComboBox owns a LineEdit, Button, and nonmodal Popup<ListView>. Arbitrary text is allowed;
typing filters choices by substring (ASCII case-insensitive). F4/Alt-Down or the arrow
button opens choices; activation maps the filtered row back to the original item index.
`on_change` reports text, `on_select` reports committed identity. Editing a committed
label clears identity; committing a new item emits one selection change. `editor` exposes
read-only, validation, clipboard, and other shared input configuration. Closing the
popup returns focus to the editor. Replace items through the owning control's `set_items`.

## Tabs

```cpp
auto pages = ox::Tabs{
    std::tuple{ox::LineEdit{}, ox::TextBox{}},
    std::vector<std::string>{"Name", "Notes"},
};
```

Tabs owns a Stack of ordinary tuple/range pages and a tab strip. Only the selected page
participates in layout, focus, timers, input, and painting. `select`, `selected_index`,
and `on_select(optional<size_t>)` match Stack. `labels` is read-only; `set_labels` validates
its count against pages. After editing a dynamic pages container, update labels and
refresh. Ctrl-Tab/Shift-Ctrl-Tab or Ctrl-PageDown/PageUp switches pages even from an editor;
arrows switch when the strip is focused. Switching a focused page focuses the first
eligible control on its replacement. A long strip scrolls to show the selected tab.

## Root overlays, popups, and dialogs

Keep popups and dialogs in the owner's widget tree. Connect an action to `show()`
to open one; Escape rejects this dialog:

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

Popup owns its content and borrows a move-aware `anchor`. Choose a placement
(`Center`, `Below`, or `Right`) and optional size/offset; the panel clamps to the
screen and flips above or left when needed. `show` and `hide` emit `on_open` and
`on_close` only on a change.

Popups are modal and dismissible by default; set `modal = false` for a nonmodal
popover. Escape, outside-click, and Tab dismissal are configurable. Tab closes an
ordinary popup before moving focus; Dialog keeps Tab inside its content.

Dialog composes Popup and Bordered content. It blocks outside-click dismissal and
provides `accept`/`reject` with `on_result(DialogResult)`; Escape rejects. Compose action
buttons from ordinary Row/Button values and connect them to those commands. Label-only
dialogs retain a keyboard focus fallback for Escape/Tab.

A modal popup confines input, focus traversal, and widget timers to its subtree.
Closing it restores the preceding eligible focus. See [routing internals](internals/routing-and-focus.md#overlays-and-modal-scopes)
for overlay geometry, nested scopes, and pointer capture.

## Menus

`MenuItem` contains a shared `Command` and child items. A null command is a
separator. Label, enabled/checked state, action, and key bindings belong to that
command; shortcut labels are generated from the keys that actually dispatch it.
A Menu composes ListView navigation, scrolling, and typeahead. Enter/click activates;
hover/Right opens submenus and Left returns. Nested `on_trigger` values are index-path
snapshots. Leaf actions are copied before notifications, so closing or deleting the
menu during a callback is safe. Commands retain normal modal shortcut eligibility.

```cpp
auto open = ox::command({.text="Open", .action=[] { return ox::EventResponse{}; },
                         .keys={{U'o', {.ctrl=true}}}});
auto bar = ox::MenuBar{{{"File", {{.command=open}}}}};
```

See [shared commands](commands.md) for reuse across menus, palettes, and KeyHelp.

F10 focuses MenuBar from anywhere in its input scope and dismisses an open menu.
Alt plus a menu label's first letter opens or switches to that menu.
Click/Enter/Down opens a menu; Left/Right moves across the bar and switches open
menus. Use `open(index)` and `close()` for programmatic control. `on_trigger`
prepends the bar menu index to the item's path. Reopening starts at the top level.

The [showcase](../examples/showcase.md) includes lists, dropdowns, editable
choices, radio groups, tabs, menus, and dialogs.
