# Selection widgets and application structure

Include `<ox/ox.hpp>` or the individual headers `selection.hpp`, `listview.hpp`,
`select.hpp`, `tabs.hpp`, `popup.hpp`, and `menu.hpp`.

## On this page

- [Shared row components](#shared-row-components)
- [ListView](#listview)
- [Select and ComboBox](#select-and-combobox)
- [Tabs](#tabs)
- [Root overlays, popups, and dialogs](#root-overlays-popups-and-dialogs)
- [Menus](#menus)

## Shared row components

`RowSelection` is an ordinary value containing sorted selected `rows`, a keyboard
`current` row, and a range `anchor`. `select_rows` composes replace, extend, toggle,
and cursor-only actions; an optional predicate excludes disabled rows. `row_navigation`
interprets arrows, page keys, Home, and End, while `row_target` finds an eligible row.
`visible_rows` calculates a clipped half-open interval. `paint_row_text` shares Unicode
alignment, whole-grapheme clipping, ellipsis, and caller-owned TextLayout storage.
DataTable uses the same navigation and visible-row/text painting functions.

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

Scrolling is independent of selection: `content_size`, `viewport_size`, `scroll_offset`,
`set_scroll_offset`, and `on_scroll(Point, Area, Area)` match the other scrollable widgets.
Navigation and explicit selection reveal the cursor. Selecting an already selected row
can restore its viewport without emitting another selection change. `link(list, bar)` supports either axis and returns both
connection identifiers. Rendering visits only clipped rows and reuses per-item layouts.

RadioGroup uses string items and optional selection indices. It supports
Home/End/PageUp/PageDown, wheel scrolling, and `set_item_enabled(index, bool)`.
Selection reveals the chosen row, and disabled rows are skipped. `scroll_offset()` and
`set_scroll_offset(Point)` control the vertical viewport; x is always zero.

## Select and ComboBox

```cpp
auto choice = ox::Select{{.items = {{"Red"}, {"Green"}, {"Blue"}}, .selected = 1}};
auto editable = ox::ComboBox{{
    .items = {{"Apple"}, {"Apricot"}, {"Banana"}},
    .editor = {.placeholder = "Type a value"},
}};
```

Select owns a Button and Popup<ListView>. Opening reveals the committed row, including
in long lists; identical captions retain distinct indices. Navigation previews a choice; activation
commits its index and emits `on_change(optional<size_t>)`. Escape, Tab, or an outside click
cancels the preview. Tab then advances through the surrounding form. Disabled choices cannot be committed. `select`, `set_items`,
`selected_index`, and `items` provide explicit commands/read access. Button activation,
Alt-Down, or F4 opens it. `button` and `popup` expose composition/configuration.

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

An ordinary child with `overlay = true` is promoted to the root layer. Its bounds are
global, it bypasses intermediate clipping, and it paints after the entire normal tree.
Logical ownership remains intact for event bubbling and modal eligibility; nonmodal
choices inside a dialog therefore remain part of that dialog's input scope.
The application arranges overlay surfaces to the root extent; their children describe
actual panels. Owner visibility still applies. `bring_to_front` raises an attached
overlay; opening a Popup does this automatically. Ordering belongs to the application.

Popup owns its content, a borrowed move-aware `anchor`, placement (`Center`, `Below`,
`Right`), optional requested size, and offset. Panel geometry clamps to the screen and
flips above/left when needed. Before drawing content, Popup clears the complete panel
rectangle with the theme's raised surface, including gaps between children. The rest
of the screen remains visible. `show`/`hide` are idempotent and emit `on_open`/`on_close`.
Escape, outside-click, and Tab dismissal are configurable. Tab closes ordinary popups
before normal focus traversal; Dialog disables this behavior to keep Tab inside it. Defaults are modal and dismissible;
set `modal = false` for a nonmodal popover. Keep Popup in the owner's child enumeration;
no separate lifetime registry or external owner is required.

```cpp
auto root = ox::Column{
    ox::Button{{.label = {.text = "Open"}}},
    ox::Dialog{ox::Column{ox::Label{"Enter a name"}, ox::LineEdit{}}, "Name"},
};
auto& dialog = std::get<1>(root.children);
dialog.requested_size = ox::Area{40, 7};
```

Dialog composes Popup and Bordered content. It blocks outside-click dismissal and
provides `accept`/`reject` with `on_result(DialogResult)`; Escape rejects. Compose action
buttons from ordinary Row/Button values and connect them to those commands. Label-only
dialogs retain a keyboard focus fallback for Escape/Tab.

A visible `modal` widget confines input, Tab traversal, pointer capture, and widget timers
to its subtree. Nested scopes save move-aware focus identities and restore the preceding
eligible control when closed. If that control has expired, normal focus recovery applies.
Capture outside a newly opened modal is cancelled. Global widget `shortcuts` callbacks
are also limited by scope; they return normal EventResponse effects. Empty functions
participate in no shortcut dispatch. Explicit `request_focus(target)` lets an attached
container focus a newly selected descendant before its attachment is reconciled.

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

MenuBar composes buttons and root popups. F10 focuses it from anywhere in its input scope;
Alt plus a menu label's first letter opens that menu; Alt shortcuts switch menus
while open, and F10 dismisses the menu. Click/Enter/Down opens a menu;
Left/Right moves across the bar and switches top-level menus while open. `open(index)`, `close`, and `menu(index)` expose commands
and content. `on_trigger` prepends the bar menu index to the item's path. Closing resets
submenus so reopening begins at the top level.

The [showcase](../examples/showcase.md) includes lists, dropdowns, editable
choices, radio groups, tabs, menus, and dialogs.
