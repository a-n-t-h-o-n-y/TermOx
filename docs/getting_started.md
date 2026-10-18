# Getting started

Create a small project, add TermOx as a source dependency, and run the program from
the root README. The finished project has its own executable and CMake target.

## Requirements

- Git and CMake 3.18 or newer.
- A C++20 compiler and standard library with `std::format` and coroutine support.
- A terminal for interactive input and output.
- Network access for the initial clone and CMake dependency downloads.

TermOx's CMake target brings in pinned revisions of
[Escape](https://github.com/a-n-t-h-o-n-y/Escape),
[Signals Light](https://github.com/a-n-t-h-o-n-y/signals-light), and
[zzz](https://github.com/a-n-t-h-o-n-y/zzz). Their revisions are recorded in
[CMakeLists.txt](../CMakeLists.txt).

The shell commands below use a POSIX-style shell.

## 1. Create the project

From a directory where you keep source projects:

```sh
mkdir -p my_app/external
cd my_app
git clone --branch library-refactor https://github.com/a-n-t-h-o-n-y/TermOx.git external/TermOx
```

The commands use the repository's current development branch. Create `main.cpp` in
`my_app/` and paste the **complete C++ block**, including `main()`, from the
[README counter](../README.md#a-counter). No separate counter example file is needed.

Create `CMakeLists.txt` next to `main.cpp`:

```cmake
cmake_minimum_required(VERSION 3.18)
project(my_app LANGUAGES CXX)

add_subdirectory(external/TermOx)
add_executable(my_app main.cpp)
target_compile_features(my_app PRIVATE cxx_std_20)
target_link_libraries(my_app PRIVATE TermOx)
```

Your source directory should now look like this:

```text
my_app/
  CMakeLists.txt
  main.cpp
  external/
    TermOx/
      CMakeLists.txt
      include/
      src/
      ...
```

The `TermOx` target supplies include paths and dependency links. This uses the source
tree directly; the repository does not provide an installable package for
`find_package(TermOx)`.

## 2. Configure and build

Run these commands from `my_app/`:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target my_app
```

CMake fetches the pinned dependencies during the first configure. Building `my_app`
also builds the library; it does not build the repository's example apps or tests.
The directory tree does not require any global include paths or manual library lists.

These commands assume a single-configuration generator such as Ninja or Makefiles.
With a multi-configuration generator, build with `--config Release` and run the
executable from `build/Release/` instead. Windows executables have an `.exe` suffix.

## 3. Run it

```sh
./build/my_app
```

Click Increment, or use Tab to focus the button and Enter to activate it. The number
should increase. Ctrl-C exits and restores the terminal. Run this from a terminal
shell rather than an IDE's output panel.

The Column owns the label and button. The connection supplies the live label to its
callback, and `set_value()` requests the update. Read
[Connections and lifetime](connections.md) for callback ownership and
[Layout and containers](containers.md) for value/reference storage and size policies.

## Common build and run problems

| Symptom | What to check |
| --- | --- |
| `<format>` is missing, or `std::format` is unavailable | Check the standard library as well as the compiler. C++20 mode cannot supply a missing library facility. Some libc++ releases require `-DCMAKE_CXX_FLAGS=-fexperimental-library`; use that only if your toolchain requires it. |
| CMake uses a different compiler from the one selected in the shell | Compiler choice is cached. Configure a new directory with `cmake -S . -B build-other -DCMAKE_CXX_COMPILER=/path/to/compiler`. |
| A dependency cannot be downloaded | The initial configure needs access to the pinned Git repositories. Check the first fetch error and network access, then rerun configure. |
| CMake cannot find `external/TermOx` | Check the directory structure and run configure from `my_app/`, not inside the dependency. |
| The executable is not at `build/my_app` | Check the selected generator and configuration; multi-configuration builds place it under `build/Release/`. |
| Input, colors, or terminal restoration do not work in an IDE output panel | Run the executable in a terminal. Ctrl-C is the app's normal terminal exit path. |

## Explore the Showcase

From `my_app/`, you can also build the larger app supplied by the dependency:

```sh
cmake --build build --target TermOx.examples.showcase
./build/external/TermOx/examples/TermOx.examples.showcase
```

The [Showcase walkthrough](../examples/showcase.md) lists its pages and controls.
For a standalone checkout, the [root README](../README.md#build-and-run-the-showcase)
has the shorter build sequence. The [apps index](../examples/README.md) lists games
and tools, and the [guides](README.md) cover scrolling, forms, background work, and
custom widgets. Use the generated [API reference](https://a-n-t-h-o-n-y.github.io/TermOx/)
for individual members.
