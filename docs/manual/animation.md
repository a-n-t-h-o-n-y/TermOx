# Animation

The Animation system in CPPurses allows Timer Events to be sent to any Widget at
a some chosen interval. The Animation Engine contains its own Event Loop,
running in a separate thread.

## Methods

### `void Widget::enable_animation(Animation_engine::Period_t period)`

This will start sending Timer Events to the called on Widget every `period`. Be
careful with extremely small periods, this could lock up the UI.

### `void Widget::enable_animation(std::function<Animation_engine::Period_t()> period_function)`

This will start sending Timer Events to the called on Widget at an interval
determined by the `period_function`. This function will be called on every
iteration to get a new period to wait for. This allows for variable timers.

### `void Widget::disable_animation()`

This will stop any Timer Events from being sent to the called on Widget.

## Pipe Methods

- `animate(Animation_engine::Period_t period)`
- `animate(std::function<Animation_engine::Period_t()> period_func)`
- `disanimate()`
