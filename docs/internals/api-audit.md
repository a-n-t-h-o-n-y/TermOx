# API audit record

This record describes the checks run during the drawing, layout, and connection
audit. Counts refer to that revision; use [Contributing](../contributing.md) for
current build and test commands.

## Contracts

- [Drawing contracts](frame-and-canvas.md#drawing): borrowed canvases,
  validation, glyph ranges, and margin fills.
- [Suspended](../containers.md#suspended): child ownership, refresh, and geometry.
- [Connections and lifetime](../connections.md): subscription ownership and tracked
  widget arguments.

## Recorded validation

Validation: 221 Release cases (including eight allocation-fault cases), all seven
example builds, nine PTY smoke checks, independent compilation of all 34 public
headers, and compilation of the README/guide examples pass. ASan+UBSan passes 122
non-exception cases with every TermOx source instrumented and the unchanged Escape
Release library linked. Deliberate exception tests run in Release because the installed
sanitizer runtime cannot unwind C++ exceptions.
