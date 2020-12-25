# Layouts

A Layout is a special type of Widget that is able to hold other Widgets within
itself. The contained Widgets are referred to as 'child' Widgets.

Layouts are responsible for resizing and moving their children in reponse to
certain events, such as when the Layout's position or size is changed.

Layouts act as containers of pointers, and like containers, they are
parameterized by the type held within the container. This type is defaulted to
`Widget`, but any derived type of `Widget` can be used. With a more derived
type, the Layout will give you access to children via that more derived type,
but will restrict the type of Widgets that can be held by that Layout. Any
Widget that is of that type or further derived can be added to the Layout, but
access will only be provided via the parameterized type.

## Linear Layouts

TermOx provides two Linear Layouts: `Horizontal` and `Vertical`.

These layouts try to fit as many child Widgets on screen as their Size Policies
allow. Widgets are ordered physically in the same order they are added to the
Layout with the `make_child` method.

Horizontal Layouts order from left to right.

Vertical Layouts order from top to bottom.

## Stack Layout

A Stack Layout is only able to display one child Widget at a time. Each Widget
is a 'page' in the stack of Widgets, where one page is displayed at a time.

There is also a `Menu_stack` Layout that provides a menu interface to select a
page and display it.

## Layout Modifiers

TermOx provides a few 'Layout Modifiers' that build on top of the above Layout
types to provide a specific behavior.

### Array

An `Array` type Layout provides a fixed number of child widgets, all of the same
type. All child Widgets are constructed with the given Array constructor
parameters.

```cpp
auto five_buttons  = Array<layout::Horizontal<Button>, 5>;
auto ten_textboxes = Array<layout::Stack<Textbox>, 10>;
```

The first template parameter is the Layout type to be used, and the second is
the number of child Widgets to build.

### Tuple

A `Tuple` type Layout helper is available, this template takes a Layout type,
and a list of Widgets that will be placed within that Layout. This is helpful to
quickly create collections of Widgets, and it also provides access to the held
Widgets by their full type. This type will only default construct its child
Widgets at the moment.

```cpp
using Btns = Array<layout::Horizontal<Button>, 2>;
using App  = Tuple<layout::Vertical<>, Textbox, Btns>;

auto app      = App{};
Textbox& txbx = app.get<0>();
Button&  btn0 = app.get<1>().get<0>();
```

### Set

A `Set` Layout will take a Layout type, a `Projection` function and a
`Comparison` function. This will turn the Layout into a 'multi-set' like data
structure. An ordering is created with the `Comparison` operation, which is fed
the output of a given child Widget applied to the `Projection` function. The
ordering is used to physically order the Widgets within the given Layout type.

```cpp
auto get_label                = [](Button const& b){ return b.get_label(); };
auto alphanum_ordered_buttons = layout::Set<layout::Vertical<Button>, decltype(get_label)>{};
alphnum_ordered_buttons.insert(button(L"b"));
alphnum_ordered_buttons.insert(button(L"a"));
alphnum_ordered_buttons.insert(button(L"z"));
alphnum_ordered_buttons.insert(button(L"y"));
```

### Selecting

A `Selecting` Layout modifier will add the concept of a 'selected child' to a
Layout, where there is only ever a single selected child, and the `select()` and
`unselect()` methods of the child are called on. This relies on the child type
of the Layout having those two methods. The template parameter is the type of
Layout this will be built upon. There are also methods to change the user input
that will change the selected child.

A `Selectable` template class is provided to wrap any Widget type with a
`select()` and `unselect()` method that can be provided as lambdas, or defaulted
to adding the `Trait::Standout` upon selection.

```cpp
using Check      = Selectable<HCheckbox_label>;
using Check_list = Array<layout::Selecting<layout::Vertical<Check>>, 15>;
```

This creates a type that will have 15 `HCheckbox_label`es, arranged vertically,
and each `HCheckbox_label` will get the `Trait::Standout` when it is selected.
