# Layout calculation

`<ox/core/layout.hpp>` provides sizing and placement functions without widgets,
coroutines, event loops, or terminal I/O. `SizePolicy` describes one axis with an
integer minimum, integer maximum, and relative growth weight (`flexibility`).

```cpp
#include <array>
#include <ox/core/layout.hpp>

auto policies = std::array{
    ox::SizePolicy::fixed(2),
    ox::SizePolicy::max(4),
    ox::SizePolicy::flex(),
};
auto lengths = ox::distribute_length(policies, 12); // {2, 4, 6}
auto rectangles = ox::layout_row(policies, {12, 3});
// {{at: {0,0}, size: {2,3}}, {at: {2,0}, size: {4,3}},
//  {at: {6,0}, size: {6,3}}}
```

Functions accept `std::span<SizePolicy const>` and return owned vectors. Arrays and
vectors of policies work directly. Inputs are not modified and no storage is borrowed
by the result.

## Allocation rules

Negative available space, negative minimums, maximums below minimums, and negative or
nonfinite weights throw `std::invalid_argument`. Validation happens even when the
available length is zero. An empty input produces an empty result.

When minimums fit, each child receives its minimum. Remaining space is shared in
proportion to positive weights. Children stop growing at their maximum, and their
share is redistributed among the remaining eligible children. Zero-weight children
never grow, including during rounding. If all eligible children reach their maximum,
unused space remains at the end of the layout. Exactly fitting minimums are preserved.

When minimums do not fit, every minimum is scaled by `available / sum(minimums)`.
This includes fixed-size children: impossible minimums yield to the container bounds.
A zero minimum stays zero, and the resulting lengths consume all available space.
For example, fixed lengths `{2, 6}` in four cells become `{1, 3}`.

In either case, fractional lengths are rounded down and remaining cells go to the
largest fractional remainders, with ties resolved in input order. Three equally
flexible children in five cells receive `{2, 2, 1}`. The calculation is deterministic;
rounding can change which child gets an extra cell when available space changes.

Shrink ratios use integer arithmetic. Growth calculations use `long double` for the
floating-point weights, then produce integer lengths. Growth caps are sorted once,
and remaining weights are summed from the end to preserve tiny weights after large
weights saturate. Work is bounded by the number of policies: O(n log n) time and O(n)
storage. There are no loops proportional to the terminal size or amount of shortage.
Length sums and placement offsets do not overflow at `INT_MAX` available dimensions.

## Placement and widget integration

`layout_row(policies, area)` places rectangles from left to right with the full area
height. `layout_column(policies, area)` places them from top to bottom with the full
area width. Both start at `{0, 0}`. Results never extend outside the supplied area;
zero-length children are retained in the result. Negative area dimensions throw.

`Row` and `Column` collect visible children and their policies once, calculate all
rectangles, then apply them in order. Invisible children are excluded and retain their
previous geometry. Each visible child's `on_arrange(previous_bounds)` receives its previous rectangle
after `arrange(child, bounds)` assigns the new geometry. Notifications also run when the
size is unchanged, so nested layouts can respond to changed policies or visibility.
Invalid sizing inputs are rejected before any child geometry is changed. Exceptions
from a child's layout callback propagate; earlier callbacks are not rolled back.

`centered_rect(available, requested)` clamps the requested width and height to the
available area, then centers the result. Odd spare cells remain at the right or bottom.
`Suspended` uses an explicit requested size rather than consulting the child's size
policy; [the container guide](../layout.md#centered-content) shows its use.

## Refresh and callback mutations

Row and Column share one implementation over the caller's tuple or sized forward
range. [Core concepts](../core-concepts.md#widgets-and-ownership) describes ownership
and child access.

The public refresh workflow is described in [Layout](../layout.md#changing-a-layout).
`arrange(layout, bounds)` also always refreshes descendants, even when the bounds have
not changed. Refresh the layout whose children changed; a refresh from outside arrangement
can also start at an ancestor to propagate through nested layouts. Public field edits do
not observe themselves automatically.

Arrangement collects visible children without allocating a coroutine. Policies and
private identity slots use reusable vectors. Slots follow moves and clear on destruction
without owning widgets. Membership comparisons avoid copying handles or locking weak
pointers when membership is unchanged. If available size and policies match the
last successful computation, rectangles are reused; every live visible child still gets
its arrangement hook. When geometry/policies change, the existing layout_row/layout_column
functions compute a fresh validated plan before child hooks run. Growth, shrinking, caps,
weights, and deterministic rounding retain the core layout algorithm.

Hooks can relocate children through vector growth/removal, move or destroy the parent,
or request another refresh. A refresh during an active pass is deferred. If membership,
visibility, policies, or size changed, the old plan is discarded and the new plan applied;
redundant unchanged requests settle without recursive arrangement. Callback mutations
also get checked at the end of the pass. Use refresh after removing a still-live child
into another container to discard its old pending arrangement. Mutation callbacks must
settle rather than continually alter their own layout constraints. Changes to nested
containers during a hook should refresh that nested layout directly.

Identity slots are resolved after each callback; no raw child pointer survives into
the next hook. Exceptions reset the arrangement state, allowing a later refresh to recover.
Validation/allocation failures while constructing the plan leave child geometry unchanged.
A throwing child hook can leave an already-applied prefix of child geometry, as with
ordinary arrange. The parent's bounds are updated before its hook is invoked.

Borrowed child enumeration uses one coroutine frame rather than nested forwarding
coroutines for tuple children. Keep the container stable while iterating that borrowed
range. Arrangement itself does not use the borrowed generator.

