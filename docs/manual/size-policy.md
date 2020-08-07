# Size Policy

A Size Policy informs a Linear Layout how a single dimension of a Widget should
be sized. Each Widget contains a `width_policy` and a `height_policy`, both are
`Size_policy` objects.

Size Policies are not used by `layout::Stack`, it can only display a single
Widget at a time.

There are seven defaults for a Size Policy.

## Size Policy Defaults

### Fixed

`void fixed(std::size_t hint)`

The length is always fixed to exactly `hint` cells. If there is not enough space
for this length, the Widget will be disabled, unless ` can_ignore_min(...)` is
enabled.

### Minimum

`void minimum(std::size_t hint)`

`hint` will be the minimum length, relying on its Stretch Factor for its length.
`can_ignore_min(...)` can be used with this default.

### Maximum

`void maximum(std::size_t hint)`

`hint` will be the maximum length, relying on its Stretch Factor for its dynamic
length.

### Preferred

`void preferred(std::size_t hint)`

`hint` will be the preferred size, the Stretch Factor determines if it should
shrink or expand from the hint.

### Expanding

`void expanding(std::size_t hint)`

Same as `preferred`, but will expand into extra space before other policies.

### Minimum Expanding

`void minimum_expanding(std::size_t hint)`

`hint` is the minimum, will expand into extra space before other policies.
`can_ignore_min(...)` can be used with this default.

### Ignored

`void ignored()`

Stretch Factor is the only consideration for this policy.

## Size Policy Parameters

These parameters define a Size Policy, some are set by the above default methods
as a convinience, but they can be altered directly as well, if one of the
defaults does not fit your use case.

### Stretch Factor

`void stretch(double)`

The Stretch Factor is used to distribute space between sibling Widgets.

A ratio of the current policy's stretch over the sum of all sibling's stretch,
including the current policy's, is created. The ratio is used to expand/shrink
beyond the given hint, depending on how much space is in the Layout.

For two adjacent Widgets in a Horizontal Layout, if one has a width policy
stretch factor of 1 and the other has a stretch factor of 2, then the second
Widget will have twice the width of the first one.

The default Stretch Factor is 1.

### Hint

`void hint(std::size_t)`

The hint is used as a starting point and perferred value for the length.

### Min

`void min(std::size_t)`

The minimum length that the policy will allow. For situations where there isn't
enough screen space to display the minimum length, it can be ignored with the
`can_ignore_min(...)` method. Otherwise, if there is not enough space to display
the minimum length, the Widget is disabled.

### Max

`void max(std::size_t)`

The maximum length that the policy will allow.

### Can Ignore Min

`void can_ignore_min(bool enable)`

If there is not enough space to display at least the minimum length, setting
this to `true` will allow the length to be smaller than the `min` value. When
multiple Widgets within the same layout have this enabled, the last Widget in
the layout with this enabled is the one to shrink.

### Passive

`void passive(bool enable)`

If a Layout enables this, then it will set the sum of its children's Size Policy
hints as its own fixed size. This assumes that all children have a fixed Size
Policy.

## Pipe Methods

### Width

- `fixed_width(std::size_t hint)`
- `minimum_width(std::size_t hint)`
- `maximum_width(std::size_t hint)`
- `preferred_width(std::size_t hint)`
- `expanding_width(std::size_t hint)`
- `minimum_expanding_width(std::size_t hint)`
- `ignored_width()`
- `width_hint(std::size_t hint)`
- `width_min(std::size_t min)`
- `width_max(std::size_t max)`
- `width_stretch(double stretch)`
- `can_ignore_width_min()`
- `cannot_ignore_width_min()`
- `passive_width(bool x = true)`

### Height

- `fixed_height(std::size_t hint)`
- `minimum_height(std::size_t hint)`
- `maximum_height(std::size_t hint)`
- `preferred_height(std::size_t hint)`
- `expanding_height(std::size_t hint)`
- `minimum_expanding_height(std::size_t hint)`
- `ignored_height()`
- `height_hint(std::size_t hint)`
- `height_min(std::size_t min)`
- `height_max(std::size_t max)`
- `height_stretch(double stretch)`
- `can_ignore_height_min()`
- `cannot_ignore_height_min()`
- `passive_height(bool x = true)`
