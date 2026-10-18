# Showcase

Build and run from the repository root:

```sh
cmake -S . -B build
cmake --build build --target TermOx.examples.showcase
./build/examples/TermOx.examples.showcase
./build/examples/TermOx.examples.showcase --theme ocean
```

The six pages cover animation and values; a validated form and selection controls;
filterable lists, tables, trees, logs, and split panes; Unicode text editing and
labels; a [BMP viewer](bmp-viewer.md) with file opening, zoom, and image
details; and file picking. Menus, a command palette, dialogs,
keyboard help, linked scrollbars, and responsive layouts connect the pages.

Use Tab for focus, Ctrl+Tab to change pages, Ctrl+P for commands, F1 for expanded
help, F2 to cycle themes, F3 for motion, and Ctrl+C to exit. Choose a theme from
the toolbar, the Actions → Theme menu, or the command palette. Selecting a preset
also selects its ASCII setting; the toolbar's ASCII checkbox can override it.
Motion has its own checkbox. The example offers deterministic headless checks:

```sh
./build/examples/TermOx.examples.showcase --check
./build/examples/TermOx.examples.showcase --dump --page 2 --width 120 --height 38
./build/examples/TermOx.examples.showcase --json --light
```

On POSIX systems, run `python3 -B tests/smoke_showcase.py build` to exercise input,
resizing, overlays, and terminal restoration through a pseudo-terminal.

`--theme` accepts `dark`, `light`, `terminal`, `ocean`, `ember`, `violet`, `paper`,
and `high-contrast`. `--light` and `--terminal` are shortcuts for those presets.
`--page` accepts 0–5. Dumps disable motion
and print actual frame contents; JSON includes each cell's text, width, and RGB
colors when present. `--check` exercises every page in every selectable theme at
large, narrow, tiny, and zero sizes, plus validation, filtering, and overlays.
