# Layouts

A Layout is a Widget that owns other Widgets. The contained Widgets are referred
to as Children Widgets.

Layouts are responsible for resizing and moving their children in reponse to the
Layout's position and size changing.

## Linear Layouts

CPPurses provides two Linear Layouts: Horizontal and Vertical.

These layouts try to fit as many child Widgets on screen as Size Policies allow.
Widgets are ordered physically in the same order they are added to the Layout
with the `make_child` method.

Horizontal Layouts order from left to right.

Vertical Layouts order from top to bottom.

- [Horizontal Layout](horizontal.md)
- [Vertical Layout](vertical.md)

## Stack Layout

A Stack Layout is also provided by CPPurses. This Layout is only able to display
one child Widget at a time. This is similar to a tabbed browser, where one
webpage is shown at a time, but the tabs are not provided here.

There is also a Menu_stack Layout that provides a menu interface to select a
page and display it.

- [Stack Layout](stack.md)

## Layout Modifiers

### Selecting

A Selecting Layout modifier will add the concept of a 'selected child' to a
layout, and provides user input bindings to change the selected child. This
relies on the Child type of the Layout having a `select()` and `unselect()`
method method.

- [Selecting Layout Modifier](selecting.md)

### Set

A Set Layout modifier will take a Layout type, a Projection function and a
Comparison function. This will turn the Layout into a 'set' like data structure,
where the Widget is transformed into some comparable type with the Projection
function, then that result is used with the Comparison operation to create an
ordering for the Widgets, which is used to order the widgets physically within
the Layout. This can be useful if you need Widgets ordered in some way within a
layout.

- [Set Layout Modifier](set.md)
