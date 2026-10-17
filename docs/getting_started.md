# Getting started

Build the counter first, then use the same source in your own project.

## Requirements

- Git and CMake 3.18 or newer.
- A C++20 compiler and standard library with `std::format` and coroutine support.
- A terminal for interactive programs.
- Network access on the first configure, when CMake fetches dependencies.

TermOx links [Escape](https://github.com/a-n-t-h-o-n-y/Escape),
[Signals Light](https://github.com/a-n-t-h-o-n-y/signals-light), and
[zzz](https://github.com/a-n-t-h-o-n-y/zzz). Their revisions are pinned in the root
[CMakeLists.txt](../CMakeLists.txt).

## Build an example

```sh
git clone https://github.com/a-n-t-h-o-n-y/TermOx.git
cd TermOx
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target TermOx.examples.counter
./build/examples/TermOx.examples.counter
```

These commands use a single-configuration generator, such as Makefiles or Ninja.
With a multi-configuration generator, add `--config Release` to the build command
and look for the executable in `build/examples/Release/` (with `.exe` on Windows).

Click Increment, or press Tab to focus it and Enter or Space to activate it.
Ctrl-C exits and restores the terminal. The executable needs a terminal; run it
from a shell rather than an IDE's output panel.

Examples are excluded from the default build. Build them by target name; the
[examples index](../examples/README.md) lists the available targets.

If the compiler cannot find `<format>`, check the standard library as well as the
compiler. Enabling C++20 alone does not add missing library facilities. On the
Apple Clang setup used to check this guide, configuration also needed
`-DCMAKE_CXX_FLAGS=-fexperimental-library`. That flag is specific to that toolchain.
To choose another compiler, configure a new build directory with
`-DCMAKE_CXX_COMPILER=/path/to/compiler`.

## Read the counter

The [complete source](../examples/counter.cpp) is also shown in the [README](../README.md).

`Column` stores its children by value. Here they are an `IntegerLabel` and a
`Button`. Their default height is one terminal row. Structured binding gives
references to the children already inside the layout:

```cpp
auto& [count, button] = root.children;
```

Connect the button's semantic action, `on_activate`, so the same code runs for
mouse and keyboard input:

```cpp
Connection{
    .signal = button.on_activate,
    .slot = [](IntegerLabel& count) { count.set_value(count.value + 1); },
}(count);
```

The final `(count)` makes the connection and tracks that widget. The callback
receives its current location, even if it moves, and is skipped if it has been
destroyed. The signal owns the callback; discarding the returned connection ID
does not disconnect it. Keep the ID if you need to call `signal.disconnect(id)`.

`set_value()` requests layout and painting when the value changes. Direct writes
to public fields do not necessarily request either; use the widget's setters
when available. See [the widget API](widget-api.md) for refresh and repaint rules.

`Application{root}.run()` opens the terminal and processes input. The application
borrows `root`, so the root must outlive it. In this example the application is
destroyed before the local layout.

## Use TermOx in your project

Start with this directory structure. Copy [counter.cpp](../examples/counter.cpp)
to `main.cpp`, and clone TermOx into `external/TermOx`.

```text
counter/
  CMakeLists.txt
  main.cpp
  external/TermOx/
```

Use this `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.18)
project(counter LANGUAGES CXX)

add_subdirectory(external/TermOx)
add_executable(counter main.cpp)
target_compile_features(counter PRIVATE cxx_std_20)
target_link_libraries(counter PRIVATE TermOx)
```

From `counter/`, run:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target counter
./build/counter
```

The `TermOx` target supplies its include paths and dependencies. This uses the
source tree directly; the repo does not currently provide an installable CMake
package for `find_package(TermOx)`.

## Next program

The [showcase](theme-and-motion.md#showcase) combines controls, tables, text
editing, and file selection. Use the [reference](reference.md) to look up an
interface, or the [guides index](README.md) for a topic.

For tests and terminal smoke checks, see [contributing](contributing.md).
