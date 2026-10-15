"""Build TermOx.examples.polish, then run this with its build directory."""
from pathlib import Path
import subprocess
import sys

from smoke_examples import smoke

build = Path(sys.argv[1] if len(sys.argv) > 1 else "build").resolve()
example = build / "examples" / "TermOx.examples.polish"
subprocess.run([str(example), "--check"], check=True)
smoke(example, [b"\x1b[<0;97;1M", b"\x1b[<0;97;1m", b"\x1b[B", b"\r",
                b"\x1b[<0;113;1M", b"\x1b[<0;113;1m", b"\x1b[B", b"\r",
                b"\x1b[<0;70;6M", b"\x1b[<0;70;6m", b"\x1b[<35;90;9M",
                b"\x1b[<0;10;8M", b"\x1b[<0;10;8m",
                "\x1b[200~界e\u0301\nA new line\x1b[201~".encode(),
                b"\x1bs", b"\t", b"\x1b[6~", b"\x1b[<65;20;12M"])
