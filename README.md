# TUI Library
__CPPurses__ is a Terminal User Interface(TUI) Library built on top of
[ncurses](https://www.gnu.org/software/ncurses/). It defines a framework of
Widgets, Layouts, and Events that let you focus on user interface design, while
boilerplate common to ncurses applications is handled behind the scenes.

CPPurses contains a collection of common Widgets that can be pieced together to
create a composite application. It is also possible to expand and build on top
of existing Widgets, or to create completely new Widget types, by overriding a
few virtual functions.

This is a __work in progress__. The API is not stable.

<p align="center">
  <img src="docs/images/regexplore_75.gif">
</p>


## Usage
See the [wiki](https://github.com/a-n-t-h-o-n-y/CPPurses/wiki) for examples.

## Features
- UTF-8 support.
- Mouse and keyboard input.
- Library of generic Widget types.
- Easy Widget reuse and expansion through inheritance.
- Simple and configurable layout system.
- Signals and Slots for inter-Widget communication.
- Extensible color palettes.
- Simple border drawing and manipulation.
- Experimental animation support.

## TODO
- Mouse Move Events
- Hovering Widget Layout
- Dialog Box Widget
- Tabs Widget
- More Documentation

## Installation
CPPurses depends on two header only libraries, this repo
includes them as git submodules. You'll need NCurses installed on your system.
```
git clone https://github.com/a-n-t-h-o-n-y/CPPurses.git
mkdir CPPurses/build && cd CPPurses/build
git submodule update --init --recursive --remote  # Pull in dependencies
cmake ..             # Generate Makefiles
make                 # Build library
make demos           # Build demos(optional)
sudo make install    # Install header and library archive to system defaults
```
Installing the library with CMake will place the headers and the library
archive in the standard GNU install directories.

## Using the Library
For projects using CPPurses, link with cppurses, ncurses and your system's
thread library. If you have installed the library your linker flags will be:
```
-lcppurses -lncurses -pthread
```
for most Linux distributions.

## Documentation
Reference documentation can be found [here](
https://a-n-t-h-o-n-y.github.io/CPPurses/annotated.html).

## Screenshots
<p align="center">
  <img src="docs/images/notepad_demo.png">
</p>
Notepad Demo ^

<p align="center">
  <img src="docs/images/chess_demo_1.png">
</p>
<p align="center">
  <img src="docs/images/chess_demo_2.png">
</p>
Chess Demo ^

## License
This software is distributed under the [MIT License](LICENSE.txt).
