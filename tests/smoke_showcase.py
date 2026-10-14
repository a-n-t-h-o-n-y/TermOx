"""Showcase interactions, resize, and terminal restoration through a POSIX PTY.

Build TermOx.examples.showcase, then run: python3 tests/smoke_showcase.py build
"""
from pathlib import Path
import subprocess
import sys

from smoke_examples import smoke

build = Path(sys.argv[1] if len(sys.argv) > 1 else "build").resolve()
showcase = build / "examples" / "TermOx.examples.showcase"
subprocess.run([str(showcase), "--check"], check=True)
# Open the palette, run its first matching command, let the finite sequence settle,
# change pages, expand help, switch theme, toggle motion, and exercise a modal.
smoke(showcase, [b"\x10", b"Run", b"\r", *([b""] * 9),
                 b"\x1b[9;5u", b"\x1bOP", b"\x1bOQ", b"\x1bOR",
                 b"\x10", b"dialog", b"\r", b"hello", b"\x1b"])
# Exercise indexed colors/ASCII and palette routing in the other configuration.
smoke(showcase, [b"\x10", b"files", b"\r", b"\t", b"\x1b[B"],
      ["--terminal"])
