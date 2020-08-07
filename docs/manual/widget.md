# Widget

A Widget is a unit of User Interface, it usually has a single purpose, maybe to
display the time, to take text input, or a button that can be pressed.

Widgets are the core of CPPurses, each application is built up of collections of
Widgets, these collections are called Layouts, and they will help to position
and resize your Widgets whenever the terminal screen size changes. This creates
a tree structure of Widgets, where there is a single entry point for your
application that is a Widget, owning other Widgets, which themselves may own
other Widgets, etc...

Widgets can be created by inheriting from the Widget class and overriding the
event handler virtual functions.

See [Building with Existing Widgets](building-with-existing-widgets.md) and
[Creating New Widgets](creating-new-widgets.md).

## Cursor

The Cursor can be enabled or disabled for each Widget using the `Widget::cursor`
member object.

## Border

The Widget's border can be set by accessing the `Widget::border` member object.

## Name / ID

A name can be given to individual Widget objects via the `Widget::set_name(...)`
and `Widget::name()` methods. A unique ID is generated for each Widget object,
this is accessed via the `Widget::unique_id()` method.

## Widget Library

CPPurses tries to provide a set of common Widgets, these can be built upon by
inheriting from them and expanding their functionality.

- [Button](widgets/button.md)
- [Confirm_button](widgets/confirm-button.md)
- [Toggle_button](widgets/toggle-button.md)
- [Checkbox](widgets/checkbox.md)
- [Color_select](widgets/color-select.md)
- [Cycle_box](widgets/cycle-box.md)
- [Fixed_height](widgets/fixed-height.md)
- [Fixed_width](widgets/fixed-width.md)
- [Graph](widgets/graph.md)
- [Matrix_display](widgets/matrix-display.md)
- [Menu](widgets/menu.md)
- [Read_file](widgets/read-file.md)
- [Write_file](widgets/write-file.md)
- [Spinner](widgets/spinner.md)
- [Line_edit](widgets/line-edit.md)
- [Number_edit](widgets/number-edit.md)
- [Status_bar](widgets/status-bar.md)
- [Text_display](widgets/text-display.md)
- [Textbox](widgets/textbox.md)
- [Label](widgets/label.md)
- [Log](widgets/log.md)
- [Tile](widgets/title.md)
- [Titlebar](widgets/titlebar.md)
- [Horizontal_scrollbar](widgets/horizontal-scrollbar.md)
- [Horizontal_slider](widgets/horizontal-slider.md)
- [Vertical_scrollbar](widgets/vertical-scrollbar.md)
- [Vertical_slider](widgets/vertical-slider.md)
- [Hidable.hpp](widgets/hidable.md)
- [Menu_stack](widgets/menu-stack.md)
- [Selectable](widgets/selectable.md)
- [Cycle_stack](widgets/cycle-stack.md)

## Layouts
- [Horizontal](layouts/horizontal.md)
- [Vertical](layouts/vertical.md)
- [Stack](layouts/stack.md)
- [Selecting](layouts/selecting.md)
- [Set](layouts/set.md)
