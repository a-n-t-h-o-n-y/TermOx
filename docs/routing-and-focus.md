# Event routing and focus

`Application` owns routing, hover state, and focus. All run on the event-loop thread
and can be exercised with `post()`, `step()`, and `paint()` without opening a terminal.

## Tree and geometry

`widget_tree(root)` returns a depth-first vector of `WidgetNode` values.
`widget_tree(root, storage)` replaces a caller-owned snapshot while retaining capacity;
a failed traversal may leave that storage partial. Applications build into separate
storage so traversal failures do not publish a partial tree. Each node
contains a lifetime view, its parent index, the end of its subtree, global bounds,
visible bounds, and input eligibility. The root starts at `{0, 0}`; child positions
are relative to their parents. Coordinates use checked translation and the same
rectangle intersection rules as `Canvas`.

An invisible widget (`visible = false`) hides its entire subtree. A disabled widget
(`enabled = false`) remains visible but excludes its entire subtree from input,
focus, and application-owned widget timer callbacks. Painting retains the existing
child-before-parent order, with siblings painted in their container order.

`hit_test(tree, point)` returns the deepest hit index, or no value.
`hit_path(tree, point)` returns root-to-target indices. Hits must lie inside every
ancestor's clip. Later siblings take precedence in overlapping areas. Disabled or
invisible subtrees are excluded. `focus_order(tree)` returns visible, enabled
Tab/Strong candidates in depth-first order. Zero-area and fully clipped widgets are
excluded from focus, although eligible timers can still run without a visible area.
The same snapshot representation supports attachment, hit testing, focus navigation,
and painting.

## Responses and propagation

Every widget input, hover, focus, and timer callback returns `EventResponse`:

```cpp
auto handle(ox::KeyEvent const& key) -> ox::EventResponse override
{
    if (key.action == ox::KeyAction::Release || !key.matches(U'x', {}, key.action)) { return {}; }
    ++count;
    return {.handled = true, .repaint = true};
}
```

`handled` stops input propagation; `repaint` requests painting; `quit` stops dispatch
and requests an exit code. These decisions are independent. Default widget callbacks
return an empty response. `combine(a, b)` combines effects and preserves the first
exit request. This replaces the previous void callback API completely.

Mouse input goes to the deepest hit target, then propagates through its ancestors until
handled or quitting. Each receiver gets its own local coordinates. A left press
selects the nearest Click/Strong widget along that ancestry before dispatch; other
buttons do not change focus. Mouse release and wheel events use the current hit path;
there is no implicit drag capture.

Keyboard press/release and the first paste chunk start at the focused widget and
bubble through its ancestors. Further paste chunks stay with the widget that handled
the first chunk, even if focus changes. Removing or disabling that widget drops the
rest of the stream. Plain Tab and Shift-Tab presses/repeats are consumed by focus navigation
and are not delivered to either the previous or new focus. Modified Tab shortcuts
can be handled normally by widgets.

Mouse enter/leave follow ancestry changes. Leaving notifications run from the old
leaf toward the shared ancestor; entering notifications run from the shared ancestor
toward the new leaf. Moving within the same path emits neither. Leaving the root,
changing geometry, hiding or disabling a subtree, and removing hovered widgets are
handled even if no new pointer motion occurs. Destroyed widgets receive no callbacks.

Built-ins return no repaint for unused keys, unchanged cursor movement, partial or
aborted paste, and unchanged scroll positions. Callbacks that emit opaque user signals
request repaint because their slots may change other widgets. External changes to
text, colors, or other public widget content still require `app.request_repaint()`.
Tree geometry and input eligibility changes are detected during synchronization.

## Focus ownership and lifetime

`Focus(root)` is scoped to one widget tree and returns transition effects without
applying them. Applications expose read-only access through `app.focus()` and explicit
commands through `app.set_focus(widget)` and `app.clear_focus()`. A foreign, invisible,
disabled, fully clipped, or FocusPolicy::None target is rejected before changing focus.
Transitions update the state read by `is_focused()`, call `focus_out()` and `focus_in()`,
and return repaint and quit effects for the application boundary to apply. Selecting
the same widget is a no-op.

Initial focus is the first eligible tab candidate once layout gives it a visible area.
Tab navigation wraps. With no focus, forward navigation selects the first candidate;
backward navigation selects the last. Click-only widgets are skipped by Tab order.

When focused widgets become ineligible or leave the tree, synchronization clears them
and selects the first remaining tab candidate. If there are none, focus stays empty
until one becomes eligible. Explicit `app.clear_focus()` stays empty until a click, Tab, or
explicit `app.set_focus()`. Focus order is independent for each application.

Lifetime views follow widget moves. Move assignment invalidates the overwritten
widget's old views; destruction invalidates all views. Lifetime storage is private.
`LifetimeView::try_get()` returns a pointer or null; `get()` throws when unavailable.
Typed lookup also rejects targets whose derived type was lost through slicing. Widget
removal and destruction stop its registered timers. Widget timer callbacks propagate
the widget's actual response instead of requesting unconditional repaint.

## Callback mutations

Input routing retains only the original ancestry, its bounds, and lifetime views
rather than copying the entire tree or retaining raw targets. After each
input callback, the application synchronizes attachment, focus, and hover state before
continuing propagation. Removed, destroyed, or disabled targets are skipped. Input
still propagates along the original ancestry, rather than being redirected to a new hit
or focus midway through an event. A quit result stops further callbacks immediately.

Focus callbacks can reenter focus selection. A newer transition wins over the outer
pending transition. Exceptions from user callbacks propagate during normal dispatch;
application destruction still attempts detachment if a focus-out callback throws.
The root must outlive the application and remain at the same address. The library is
single-threaded for widget mutation; background work should post an event-loop action.

Headless regression tests exercise nested clipping and translation, overlap, bubbling,
Tab navigation, focus recovery and relocation, hover transitions, callback removal,
reentrant focus changes, explicit repaint and quit, timer eligibility, and checked
coordinate arithmetic. Pseudo-terminal smoke tests cover the migrated examples.

Traversal and attachment storage are reused. An unchanged tree skips attachment
reconciliation, and an unchanged hover path needs no allocation. Focus consumes its
borrowed tree snapshot before invoking callbacks, so reentrant dispatch can safely
replace application storage. Failed attachment attempts remain pending and are retried;
a foreign widget cannot bypass ownership checks on a subsequent unchanged traversal.
See [performance benchmarks](performance.md) for measured routing workloads.

See [selection and application structure](selection-and-application.md) for ListView,
Select/ComboBox, Tabs, root overlays, menus, dialogs, modal scopes, and scoped shortcuts.
