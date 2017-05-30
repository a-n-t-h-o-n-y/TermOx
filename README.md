__CPPurses__ is a Qt inspired widget framework for NCurses, written in C++14. It
comes from a desire for a simple and modern approach to building terminal
applications. Through a set of abstractions built on top of NCurses, CPPurses
enables quick development of complex TUIs, without getting in the way of
program logic.

__Event System:__
Event loop, widget drawing, and user input.

__Signals and Slots:__
Widget to widget communication.

__Widget Library:__
Pre-made Widgets for reuse.

![alt text](src/CPPurses/docs/images/CPPurses.png)

## Features
- UTF-8 support without the need for wide characters
- (Extensible) Color Palettes
- Mouse and Keyboard Input
- Simple and configurable layout system
- Easy border drawing and manipulation
- Vanilla C++ Signals and Slots implementation
- Fully featured Textbox widget with cursor movement and scrolling

## Widgets
- Textbox
- Text_display
- Checkbox
- Push_button
- Titlebar
- Horizontal_scrollbar
- Vertical_scrollbar
- Color_chooser
- Horizontal_layout
- Vertical_layout

## Future Features
- Animation
- Mouse Move Events
- Pages
- List Widget
- Tabs Widget
- Hovering Widgets
- Dialog Box Widget
- Documentation

## Installation
CPPurses depends on two (header only)libraries I have created, this repo
includes them as git submodules. After cloning CPPurses, run
`git submodule update --init -- recursive`
to pull in the external libraries. Generated build files from CMake
will handle installation of each library and building of tests.

## Documentation
Doxygen documentation can be found [here](
https://a-n-t-h-o-n-y.github.io/CPPurses/).

## Tests
CPPurses uses google test and has support for ctest.

## License
This software is distributed under the [MIT License](LICENSE.txt).

