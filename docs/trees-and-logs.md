# Browse trees and logs

Use `TreeView` for hierarchical choices and `LogView` for a bounded stream of text.
Both support [linked scrollbars](scrolling.md) directly.

## Browse a tree

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

Each node has text, enabled/expanded flags, and children. A path is a sequence
of child indices; it identifies a node only within the current model.
Read `selected_paths()` or `current_path()`, and observe `on_select`,
`on_activate`, and `on_expand` for changes. Signal arguments are stable snapshots.

Right expands or enters a branch; Left collapses or selects its parent.
Enter/double-click activates and toggles branches. Multiple selection uses
[ListView's Shift/Ctrl controls](selection-and-application.md#listview).
Disabled nodes disable their descendants.

`set_nodes` replaces the model and clears selection. `select(path)` expands
ancestors; `set_expanded(path, false)` hides descendant selections and moves a
descendant cursor to the parent. Replacing the model from a callback cancels
pending selection and activation against the old model. See
[tree presentation](presentation.md#lists-and-trees) for row styles and guides.

## Append to a log

```cpp
auto log = ox::LogView{};
log.set_max_lines(1000);
log.append("Connected\nWaiting for events\n");
```

`append` accepts printable UTF-8 records separated by newlines. A trailing newline
does not add an extra blank record; `append("")` adds one blank line. Invalid
input rejects the whole batch. Retention must be positive; appending beyond the
limit discards the oldest records. `clear()` removes them all.

Logs follow their tail until the user scrolls upward. Use `set_follow_tail(true)`
to reveal the newest content again. While paused, appends and capacity changes
preserve the viewport and selection relative to the retained text.
`on_append` reports accepted records; `on_follow` reports changes to tail following.

The public read-only TextBox `editor` provides selection, copying, wrapping, and
styling. Editing history is disabled. Append on the application thread; worker
threads must [post their results](background-work.md). The
[streaming internals](internals/text.md#streaming-complete-lines) explain storage
and layout reuse.
