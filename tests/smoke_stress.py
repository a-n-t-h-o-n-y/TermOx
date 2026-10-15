"""Build TermOx.examples.stress, then run this with its build directory."""
import csv
import io
from pathlib import Path
import subprocess
import sys

from smoke_examples import smoke

build = Path(sys.argv[1] if len(sys.argv) > 1 else "build").resolve()
executable = build / "examples" / "TermOx.examples.stress"
small = ["--size", "300", "--widgets", "32", "--pixels", "64", "--batch", "2"]
subprocess.run([str(executable), "--check", *small], check=True)
subprocess.run([str(executable), "--check", "--all", *small], check=True)
result = subprocess.run([str(executable), "--benchmark", "--ticks", "3", *small],
                        check=True, capture_output=True, text=True)
rows = list(csv.DictReader(io.StringIO(result.stdout)))
assert len(rows) == 12 and len({row["page"] for row in rows}) == 12, result.stdout
assert all(float(row["cached_bytes"]) == 0 for row in rows), result.stdout
assert float(next(row for row in rows if row["page"] == "Frames-unchanged")["bytes_per_frame"]) == 0, result.stdout
for row in rows:
    for column in ("update_us", "step_us", "paint_us", "encode_us", "cached_encode_us"):
        assert float(row[column]) >= 0, row
print("Stress Lab: twelve headless timing scenarios and unchanged-output checks passed")
# Exercise every page, pause/resume, real editing/search, load, wrapping, all-workload
# mode, frame modes, queued events, resizing, Ctrl-C and terminal restoration.
smoke(executable, [b"\x1bp", b"\x1b2", b"\x1b[B", b"\x1b[C", b"\x1b3", b"\x1b[6~",
                   b"\x1b4", b"\t", b"command 2", b"\x1b5", b"\x1b[<0;10;7M", b"\x1b[<0;10;7m", b"\x01", b"\x7f",
                   "界e\u0301".encode(), b"\x1bw", b"\x1b6", b"\x1b[<65;5;8M",
                   b"\x1b7", b"\t", b"\r", b"\x1b8", b"\x1bm", b"\x1bm",
                   b"\x1b9", b"\x1b=", b"\x1b-", b"\x1ba", b"\x1bp"], small)
smoke(executable, [b"\x1b8", b"\x1bm", b"\x1b1", b"\x1bp"])
