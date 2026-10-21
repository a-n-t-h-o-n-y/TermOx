"""Explorer filesystem fixtures, headless behavior, and POSIX terminal interactions."""
from pathlib import Path
import json
import os
import shutil
import subprocess
import sys
import tempfile

from smoke_examples import smoke

build = Path(sys.argv[1] if len(sys.argv) > 1 else "build").resolve()
executable = build / "examples" / "TermOx.examples.explorer"
with tempfile.TemporaryDirectory(prefix="termox-explorer-") as temporary:
    root = Path(temporary)
    (root / "folder").mkdir()
    (root / "folder" / "child.txt").write_text("Nested document")
    (root / "readme.txt").write_text("Explorer preview marker\nUnicode: 界 and café\n")
    (root / "empty.txt").write_text("")
    (root / ".hidden.txt").write_text("Hidden document")
    (root / "binary.dat").write_bytes(b"\0\x1b[31munsafe")
    (root / "broken.bmp").write_bytes(b"broken")
    (root / "large.txt").write_text("x" * (256 * 1024 + 20))
    (root / "line\nbreak.txt").write_text("Escaped filename preview")
    shutil.copy(Path(__file__).parent / "../examples/assets/julia.bmp", root / "image.bmp")
    os.symlink(root / "folder", root / "folder-link")
    os.symlink(root / "missing", root / "broken-link")
    subprocess.run([str(executable), str(root), "--check"], check=True, timeout=30)

    def frame(*args):
        result = subprocess.run([str(executable), str(root), "--json", *args],
                                capture_output=True, text=True, timeout=20)
        assert result.returncode == 0, result.stderr
        data = json.loads(result.stdout)
        assert len(data["rows"]) == data["height"]
        assert all(len(row) == data["width"] for row in data["rows"])
        text = "\n".join("".join(cell["text"] for cell in row) for row in data["rows"])
        return data, text

    _, text = frame("--select", "readme.txt")
    assert "Explorer preview marker" in text and "café" in text, text
    _, text = frame("--filter", "README", "--select", "readme.txt")
    assert "1 / 11 entries" in text, text
    _, text = frame("--select", "empty.txt")
    assert "(empty file)" in text, text
    _, text = frame("--select", "binary.dat")
    assert "No text preview" in text, text
    _, text = frame("--select", "broken.bmp")
    assert "Preview unavailable" in text, text
    _, text = frame("--select", "line\nbreak.txt")
    assert "Escaped filename preview" in text and "line\\x0abreak.txt" in text, text
    data, text = frame("--select", "image.bmp")
    assert "640 x 400" in text and "▄" in text, text
    assert any(cell["bg"] != cell["fg"] and cell["text"] == "▄"
               for row in data["rows"] for cell in row)

    # Scrollable content must not displace the surrounding controls.
    (root / "long.txt").write_text("A preview line\n" * 1000)
    for width, height in [(120, 32), (80, 16)]:
        for selected in ["readme.txt", "long.txt", "image.bmp"]:
            _, text = frame("--select", selected, "--width", str(width), "--height", str(height))
            lines = text.splitlines()
            assert "File" in lines[1] and "View" in lines[1], text
            assert not lines[2].strip(), text
            assert "Path" in lines[3] and "Up" in lines[3] and "Refresh" in lines[3], text
            assert "Filter" in lines[4] and "Hidden files" in lines[4], text
            assert not lines[5].strip(), text
            assert "Folders" in lines[6] and "Files" in lines[6], text
            assert "entries" in lines[-3] or "640 x 400" in lines[-3], text
            assert "ctrl+l" in lines[-2], text
    frame("--width", "0", "--height", "0")
    frame("--width", "24", "--height", "8", "--theme", "terminal")
    bad = subprocess.run([str(executable), str(root / "missing"), "--json"],
                         capture_output=True, timeout=20)
    assert bad.returncode == 2 and b"Cannot open directory" in bad.stderr
    bad = subprocess.run([str(executable), "--width", "-1", "--json"], capture_output=True)
    assert bad.returncode == 2

    # Filter, navigate by path, open a palette command, toggle preview, and resize.
    output = smoke(executable, [b"\x06", b"readme", b"\x06", b"\x7f", b"\x0c", b"\x01",
                               str(root / "folder").encode(), b"\r", b"",
                               b"\x1b[1;3A", b"\x10", b"Light theme", b"\r",
                               b"\x1bOS", b"\x1bOS"], [str(root)])
    assert b"Nested document" in output or b"child.txt" in output
    output = smoke(executable, [b"\t", b"\x1b[B", b"\x10", b"Refresh", b"\r"],
                   [str(root), "--select", "image.bmp", "--theme", "terminal"])
    print("Explorer: previews, bounds, errors, filesystem names, and terminal interaction passed")
