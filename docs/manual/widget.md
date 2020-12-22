# Widget

Widgets are the main building block in TermOx. Each Widget attempts to have a
single, well defined purpose. This can be to display the time, to take text
input and display what was typed, to be a pushable button, etc...

Widgets are the core of TermOx, each application is built up of a hierarchy of
Widgets. [Layout](layouts.md) Widgets act as containers, grouping multiple
Widgets together into logical units. These Widgets and Layouts form a tree
structure that holds the entire application's UI state.

New Widget types can be created by inheriting from the `Widget` class and
overriding the event handler virtual functions.

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

TermOx tries to provide a set of common Widgets, these can be built upon by
inheriting from them and expanding their functionality.

[Note: These manual pages are not complete]

- [Accordion](widgets/accordion.md)
- [Button](widgets/button.md)
- [Confirm_button](widgets/confirm-button.md)
- [Toggle_button](widgets/toggle-button.md)
- [Banner](widgets/banner.md)
- [Button_list](widgets/button-list.md)
- [Checkbox](widgets/checkbox.md)
- [Color_select](widgets/color-select.md)
- [Cycle_box](widgets/cycle-box.md)
- [Graph](widgets/graph.md)
- [Hideable](widgets/hideable.md)
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
- [Tile](widgets/tile.md)
- [Label](widgets/label.md)
- [Log](widgets/log.md)
- [Tile](widgets/title.md)
- [Titlebar](widgets/titlebar.md)
- [Scrollbar](widgets/scrollbar.md)
- [Horizontal_slider](widgets/horizontal-slider.md)
- [Vertical_slider](widgets/vertical-slider.md)
- [Hidable.hpp](widgets/hidable.md)
- [Menu_stack](widgets/menu-stack.md)
- [Selectable](widgets/selectable.md)
- [Cycle_stack](widgets/cycle-stack.md)

## See Also

- [Reference](https://a-n-t-h-o-n-y.github.io/TermOx/classox_1_1Widget.html)
