# Getting started

Run the showcase, then use the README code block to start your own project.

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
cmake --build build --target TermOx.examples.showcase
./build/examples/TermOx.examples.showcase
```

These commands use a single-configuration generator, such as Makefiles or Ninja.
With a multi-configuration generator, add `--config Release` to the build command
and look for the executable in `build/examples/Release/` (with `.exe` on Windows).

Use Tab to move focus and Ctrl-Tab to switch pages.
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

## Use TermOx in your project

Start with this directory structure. Copy the C++ code block from the
[README](../README.md#a-counter) into `main.cpp`, and clone TermOx into
`external/TermOx`.

```text
my_app/
  CMakeLists.txt
  main.cpp
  external/TermOx/
```

Use this `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.18)
project(my_app LANGUAGES CXX)

add_subdirectory(external/TermOx)
add_executable(my_app main.cpp)
target_compile_features(my_app PRIVATE cxx_std_20)
target_link_libraries(my_app PRIVATE TermOx)
```

From `my_app/`, run:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target my_app
./build/my_app
```

The `TermOx` target supplies its include paths and dependencies. This uses the
source tree directly; the repo does not currently provide an installable CMake
package for `find_package(TermOx)`.

## Where to go next

Browse the [apps](../examples/README.md#apps) for games and tools built with TermOx.
Use the [reference](reference.md) to look up an
interface, or the [guides index](README.md) for a topic.

For tests and terminal smoke checks, see [contributing](contributing.md).
