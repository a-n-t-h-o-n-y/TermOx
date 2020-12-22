# Animation

The Animation system in TermOx allows Timer Events to be sent to any Widget at a
chosen interval. The Animation Engine contains its own Event Loop, running in a
separate thread.

## Methods

### `void Widget::enable_animation(Animation_engine::Period_t period)`

This will start sending Timer Events to the called on Widget every `period`. Be
careful with extremely small periods, this could lock up the UI.

### `void Widget::enable_animation(FPS fps)`

Same as above, but will calculate the `period` from the given `FPS`.

### `void Widget::disable_animation()`

This will stop any Timer Events from being sent to the called on Widget.

## Pipe Methods

- `animate(Animation_engine::Period_t period)`
- `animate(FPS fps)`
- `disanimate()`

## See Also

- [Timer Event](events.md#timer-event)
